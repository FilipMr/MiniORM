#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <sqlite3.h>
#include "IColumn.hpp"

namespace fm {

class Model {
protected:
    std::string table_name;
    std::vector<IColumn*> columns;

public:
    Model() : table_name("DefaultTable") {}

    Model(std::string t_name) : table_name(std::move(t_name)) {}

    void setTableName(std::string t_name) {
        table_name = std::move(t_name);
    }

    void register_column(IColumn* col) {
        columns.push_back(col);
    }

    // create table
    bool create_table(sqlite3* db) {
        std::string sql = "CREATE TABLE IF NOT EXISTS " + table_name + " (";
        for (size_t i = 0; i < columns.size(); ++i) {
            sql += columns[i]->getDefinition();
            if (i < columns.size() - 1) sql += ", ";
        }
        sql += ");";
        
        char* errMsg = nullptr;
        if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
            std::cerr << "Create Table Error: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return false;
        }
        return true;
    }

    // write
    bool save(sqlite3* db) {
        std::string sql = "INSERT INTO " + table_name + " (";
        std::string placeholders = ") VALUES (";

        for (size_t i = 0; i < columns.size(); ++i) {
            sql += columns[i]->getName();
            placeholders += "?"; 
            if (i < columns.size() - 1) {
                sql += ", ";
                placeholders += ", ";
            }
        }
        sql += placeholders + ");";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Save Prepare Error: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        for (size_t i = 0; i < columns.size(); ++i) {
            columns[i]->bindValue(stmt, i + 1); 
        }

        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        if (!success) std::cerr << "Save Exec Error: " << sqlite3_errmsg(db) << std::endl;
        
        sqlite3_finalize(stmt);
        return success;
    }

    // read but assumes that the first column is the primary key
    bool find(sqlite3* db, int id) {
        if (columns.empty()) return false;

        std::string sql = "SELECT * FROM " + table_name + " WHERE " + columns[0]->getName() + " = ?;";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Find Prepare Error: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        sqlite3_bind_int(stmt, 1, id);

        bool found = false;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            found = true;
            for (size_t i = 0; i < columns.size(); ++i) {
                columns[i]->loadFromSQL(stmt, i);
            }
        }

        sqlite3_finalize(stmt);
        return found;
    }

    // update each field if first column is the primary key
    bool update(sqlite3* db) {
        if (columns.empty()) return false;

        std::string sql = "UPDATE " + table_name + " SET ";
        
        // Skip ID (index 0) in the SET clause
        for (size_t i = 1; i < columns.size(); ++i) {
            sql += columns[i]->getName() + " = ?";
            if (i < columns.size() - 1) sql += ", ";
        }
        sql += " WHERE " + columns[0]->getName() + " = ?;";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Update Prepare Error: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        // Bind data values
        int bind_index = 1;
        for (size_t i = 1; i < columns.size(); ++i) {
            columns[i]->bindValue(stmt, bind_index++);
        }
        // Bind ID at the end
        columns[0]->bindValue(stmt, bind_index);

        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        if (!success) std::cerr << "Update Exec Error: " << sqlite3_errmsg(db) << std::endl;

        sqlite3_finalize(stmt);
        return success;
    }

    // delete if first column is the primary key
    bool remove(sqlite3* db) {
        if (columns.empty()) return false;

        std::string sql = "DELETE FROM " + table_name + " WHERE " + columns[0]->getName() + " = ?;";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Delete Prepare Error: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        columns[0]->bindValue(stmt, 1);

        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        if (!success) std::cerr << "Delete Exec Error: " << sqlite3_errmsg(db) << std::endl;

        sqlite3_finalize(stmt);
        return success;
    }
};

}