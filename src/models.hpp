#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <type_traits>

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



}