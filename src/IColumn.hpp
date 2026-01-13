#pragma once
#include <string>
#include <sqlite3.h>

// Abstract Column Interface to store diffrent columns
namespace fm {

class IColumn {
public:
    virtual std::string get_name() const = 0;
    virtual std::string get_definition() const = 0;
    virtual void bind_value(sqlite3_stmt* stmt, int index) const = 0;
    virtual void load_from_stmt(sqlite3_stmt* stmt, int index) = 0;
    virtual ~IColumn() = default;
};

}