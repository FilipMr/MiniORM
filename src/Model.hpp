#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <sqlite3.h>
#include "IColumn.hpp"

namespace fm {

/**
 * @brief Base class for database-backed models (lightweight ORM-style).
 *
 * A Model represents a SQLite table and a collection of registered columns.
 * Columns (implementing IColumn) typically register themselves into a Model
 * during construction (see fm::Column constructor).
 *
 * @note
 * This implementation assumes the first registered column (index 0) is the
 * primary key for operations like find(), update(), and remove().
 */
class Model {
protected:
    std::string tableName;           
    std::vector<IColumn*> columns;    

public:
    /**
     * @brief Default constructor with default table name.
     */
    Model() : tableName("DefaultTable") {}

    /**
     * @brief Constructor with a specific table name.
     */
    Model(std::string tabName) : tableName(std::move(tabName)) {}

    /**
     * @brief Setter for the table name.
     */
    void setTableName(std::string tabName) {
        tableName = std::move(tabName);
    }

    /**
     * @brief Registers a column with this model.
     * Columns call this automatically in their constructor
     *
     * @warning
     * registration order matters: columns[0] is treated as primary key.
     */
    void register_column(IColumn* col) {
        columns.push_back(col);
    }

    /**
     * @brief Creates the model's table
     *
     * Query behind:
     * @code
     * CREATE TABLE IF NOT EXISTS TableName (col1_def, col2_def, ...);
     * @endcode
     *
     * @return true on success, false on error
     */
    bool create_table(sqlite3* db) {
        std::string sql = "CREATE TABLE IF NOT EXISTS " + tableName + " (";
        for (size_t i = 0; i < columns.size(); ++i) {
            sql += columns[i]->getDefinition();
            if (i < columns.size() - 1) {
                sql += ", ";
            }
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

    /**
     * @brief Inserts the current model values as a new row.
     *
     * INSERT statement with placeholders and binds each column value:
     * @code
     * INSERT INTO TableName (c1, c2, ...) VALUES (?, ?, ...);
     * @endcode
     *
     * @return true if the INSERT executed successfully, false otherwise.
     */
    bool save(sqlite3* db) {
        std::string sql = "INSERT INTO " + tableName + " (";
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

        // SQLite bind indices are 1-based.
        for (size_t i = 0; i < columns.size(); ++i) {
            columns[i]->bindValue(stmt, static_cast<int>(i) + 1);
        }

        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        if (!success) {
            std::cerr << "Save Exec Error: " << sqlite3_errmsg(db) << std::endl;
        }

        sqlite3_finalize(stmt);
        return success;
    }

    /**
     * @brief Loads a row by primary key into this model instance.
     *
     * Assumes the first registered column is the primary key and is integer-based.
     * Executes:
     * @code
     * SELECT * FROM TableName WHERE pk = ?;
     * @endcode
     *
     * On success, each column is populated using IColumn::loadFromSQL().
     *
     * @param db Open SQLite database handle.
     * @param id Primary key value to search for.
     * @return true if a matching row was found and loaded, false otherwise.
     *
     * @warning Requires at least one registered column.
     */
    bool find(sqlite3* db, int id) {
        if (columns.empty()) {
            return false;
        }

        std::string sql =
            "SELECT * FROM " + tableName + " WHERE " + columns[0]->getName() + " = ?;";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Find Prepare Error: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        sqlite3_bind_int(stmt, 1, id);

        bool found = false;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            found = true;
            // SQLite column indices are 0-based.
            for (size_t i = 0; i < columns.size(); ++i) {
                columns[i]->loadFromSQL(stmt, static_cast<int>(i));
            }
        }

        sqlite3_finalize(stmt);
        return found;
    }

    /**
     * @brief Updates the row identified by the primary key (first column).
     *
     * Builds:
     * @code
     * UPDATE TableName SET c2=?, c3=?, ... WHERE pk=?;
     * @endcode
     *
     * The primary key column (index 0) is excluded from the SET clause and
     * is bound last in the WHERE clause.
     *
     * @param db Open SQLite database handle.
     * @return true if the UPDATE executed successfully, false otherwise.
     *
     * @warning Requires at least one registered column.
     * @note This does not check whether any row was actually modified.
     */
    bool update(sqlite3* db) {
        if (columns.empty()) {
            return false;
        }

        std::string sql = "UPDATE " + tableName + " SET ";

        // Skip ID (index 0) in the SET clause.
        for (size_t i = 1; i < columns.size(); ++i) {
            sql += columns[i]->getName() + " = ?";
            if (i < columns.size() - 1) {
                sql += ", ";
            }
        }
        sql += " WHERE " + columns[0]->getName() + " = ?;";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Update Prepare Error: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        // Bind non-PK values first.
        int bind_index = 1;
        for (size_t i = 1; i < columns.size(); ++i) {
            columns[i]->bindValue(stmt, bind_index++);
        }
        // Bind PK at the end.
        columns[0]->bindValue(stmt, bind_index);

        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        if (!success) {
            std::cerr << "Update Exec Error: " << sqlite3_errmsg(db) << std::endl;
        }

        sqlite3_finalize(stmt);
        return success;
    }

    /**
     * @brief Deletes the row identified by the primary key (first column).
     *
     * Executes:
     * @code
     * DELETE FROM TableName WHERE pk = ?;
     * @endcode
     *
     * @param db Open SQLite database handle.
     * @return true if the DELETE executed successfully, false otherwise.
     *
     * @warning Requires at least one registered column.
     * @note This does not check whether any row was actually deleted.
     */
    bool remove(sqlite3* db) {
        if (columns.empty()) {
            return false;
        }

        std::string sql =
            "DELETE FROM " + tableName + " WHERE " + columns[0]->getName() + " = ?;";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Delete Prepare Error: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        columns[0]->bindValue(stmt, 1);

        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        if (!success) {
            std::cerr << "Delete Exec Error: " << sqlite3_errmsg(db) << std::endl;
        }

        sqlite3_finalize(stmt);
        return success;
    }
};

} // namespace fm
