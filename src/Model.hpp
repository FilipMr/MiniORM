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
    Model(std::string t_name) : table_name(std::move(t_name)) {

    }

    void register_column(IColumn* col) {
        columns.push_back(col);
    }

    bool create_table(sqlite3* db) {
        std::stringstream ss;
        ss << "CREATE TABLE IF NOT EXISTS " << table_name << " (";
        for (size_t i = 0; i < columns.size(); ++i) {
            ss << columns[i]->get_definition();

            if (i < columns.size() - 1) {
                ss << ", ";
            }
        }
        ss << ");";
        
        return execute_sql(db, ss.str());
    }

    bool save(sqlite3* db) {
        std::string sql = "INSERT INTO " + table_name + " (";
        std::string placeholders = ") VALUES (";

        for (size_t i = 0; i < columns.size(); ++i) {
            sql += columns[i]->get_name();
            placeholders += "?"; 
            if (i < columns.size() - 1) {
                sql += ", ";
                placeholders += ", ";
            }
        }
        sql += placeholders + ");";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Prepare Error: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        for (size_t i = 0; i < columns.size(); ++i) {
            columns[i]->bind_value(stmt, i + 1); 
        }

        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        if (!success) std::cerr << "Execution Error: " << sqlite3_errmsg(db) << std::endl;

        sqlite3_finalize(stmt);
        return success;
    }
};

}