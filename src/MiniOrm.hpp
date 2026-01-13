#pragma once
// #include <iostream>
// #include <vector>
// #include <string>
// #include <sstream>
// #include <type_traits>
// #include <sqlite3.h>

#include "IColumn.hpp"

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
        return "TEXT";
    }
    else {
        return "BLOB";
    }
}

template <typename T>
class Column : public IColumn {
private:
    std::string name;
    std::string constraints;
    T value; 

public:
    Column(Model* owner, std::string col_name, std::string col_constraints = "") : name(std::move(col_name)), constraints(std::move(col_constraints)) {
        owner->register_column(this);
    }

    std::string get_name() const override { 
        return name; 
    }

    std::string get_definition() const override {
        return name + " " + get_sql_type<T>() + (constraints.empty() ? "" : " " + constraints);
    }

    void bind_value(sqlite3_stmt* stmt, int index) const override {
        if constexpr (std::is_same_v<T, int>) {
            sqlite3_bind_int(stmt, index, value);
        }
        else if constexpr (std::is_same_v<T, double>) {
            sqlite3_bind_double(stmt, index, value);
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
        }
    }

    void load_from_stmt(sqlite3_stmt* stmt, int index) override {
        if constexpr (std::is_same_v<T, int>) {
            value = sqlite3_column_int(stmt, index);
        }
        else if constexpr (std::is_same_v<T, double>) {
            value = sqlite3_column_double(stmt, index);
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            const unsigned char* text = sqlite3_column_text(stmt, index);
            // Handle NULLs safely
            value = text ? reinterpret_cast<const char*>(text) : "";
        }
    }
};

}