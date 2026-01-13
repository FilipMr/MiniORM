#pragma once
#include "IColumn.hpp"
#include "Model.hpp"

namespace fm {

template <typename T>
std::string getSQLType() {
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
    Column(Model* owner, std::string colName, std::string colConstraints = "") : name(std::move(colName)), constraints(std::move(colConstraints)) {
        owner->register_column(this);
    }

    //alows "user.age = 30;"
    Column<T>& operator=(const T& val) { 
        value = val; 
        return *this; 
    }

    //allows "int x = user.age;" or "cout << user.name"
    operator T() const { 
        return value; 
    }

    std::string getName() const override { 
        return name; 
    }

    std::string getDefinition() const override {
        return name + " " + getSQLType<T>() + (constraints.empty() ? "" : " " + constraints);
    }

    void bindValue(sqlite3_stmt* stmt, int index) const override {
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

    void loadFromSQL(sqlite3_stmt* stmt, int index) override {
        if constexpr (std::is_same_v<T, int>) {
            value = sqlite3_column_int(stmt, index);
        }
        else if constexpr (std::is_same_v<T, double>) {
            value = sqlite3_column_double(stmt, index);
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            const unsigned char* text = sqlite3_column_text(stmt, index);
            if (text) {
                value = reinterpret_cast<const char*>(text);
            }
            else
                value = "";
        }
    }
};

}