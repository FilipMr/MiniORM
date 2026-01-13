#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <type_traits>
#include <sqlite3.h>

namespace fm {

template <typename T>
std::string get_sql_type() {
    if constexpr (std::is_same_v<T, int>) {
        return "INTEGER";
    }
    else if constexpr (std::is_same_v<T, double>) {
        return "REAL";
    }
    else if constexpr (std::is_same_v<T, std::string>) {
        return "TEXT":
    }
    else {
        return "BLOB";
    }
}

// Abstract Column Interface to store diffrent columns
struct IColumn {
    virtual std::string get_name() {
        const = 0;
    } 
    virtual std::string get_definition() {
        const = 0;
    }
    virtual ~IColumn() = default;
};

class Model {
protected:
    std::string table_name;
    std::vector<IColumn*> columns;

    bool execute_sql(sqlite3* db, const std::string& sql) {
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
        
        if (rc != SQLITE_OK) {
            std::cerr << "SQL Error in " << table_name << ": " << errMsg << std::endl;
            std::cerr << "Query was: " << sql << std::endl;
            sqlite3_free(errMsg);
            return false;
        }
        return true;
    }

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
}

template <typename T>
class Column : public IColumn {
private:
    std::string name;
    std::string constraints;
    T value; 

public:
    Column(Model* owner, std::string col_name, std::string col_constraints = "") 
        : name(std::move(col_name)), constraints(std::move(col_constraints)) {
        owner->register_column(this);
    }

}