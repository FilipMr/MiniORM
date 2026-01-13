#pragma once
#include <string>
#include <sqlite3.h>

// Abstract Column Interface to store diffrent columns
namespace fm {

class IColumn {
public:
    virtual std::string getName() const = 0;
    virtual std::string getDefinition() const = 0;
    virtual void bindValue(sqlite3_stmt* stmt, int index) const = 0;
    virtual void loadFromSQL(sqlite3_stmt* stmt, int index) = 0;
    virtual ~IColumn() = default;
};

}