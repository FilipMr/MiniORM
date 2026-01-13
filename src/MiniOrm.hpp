#pragma once
#include "IColumn.hpp"
#include "Model.hpp"

namespace fm {

/**
 * @brief Returns the corresponding SQL type for a C++ type.
 *
 * This function maps common C++ types to SQLite-compatible
 * column types.
 *
 * @tparam T C++ type
 * @return SQL type as string
 *
 * @note
 * - int         -> INTEGER  
 * - double      -> REAL  
 * - std::string -> TEXT  
 * - other       -> BLOB  
 */
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

/**
 * @brief Represents a typed database column.
 *
 * This class provides:
 * - Automatic registration in the owning Model
 * - Type-safe assignment
 * - SQL type generation
 * - SQLite binding & loading
 *
 * @tparam T Column data type
 */
template <typename T>
class Column : public IColumn {
private:
    std::string name;        /**< Column name */
    std::string constraints;/**< SQL constraints (PRIMARY KEY, NOT NULL, etc.) */
    T value;                 /**< Stored value */

public:

    /**
     * @brief Constructs a column and registers it with its owner.
     *
     * @param owner Pointer to the owning Model
     * @param colName Column name
     * @param colConstraints Optional SQL constraints
     */
    Column(Model* owner,
           std::string colName,
           std::string colConstraints = "")
        : name(std::move(colName)),
          constraints(std::move(colConstraints)) 
    {
        owner->register_column(this);
    }

    /**
     * @brief Assigns a value to the column.
     *
     * Allows syntax:
     * @code
     * user.age = 30;
     * @endcode
     *
     * @param val Value to assign
     * @return Reference to this column
     */
    Column<T>& operator=(const T& val) { 
        value = val; 
        return *this; 
    }

    /**
     * @brief Implicit conversion to underlying type.
     *
     * Allows:
     * @code
     * int x = user.age;
     * std::cout << user.name;
     * @endcode
     *
     * @return Stored value
     */
    operator T() const { 
        return value; 
    }

    /**
     * @brief Returns column name.
     * @return Column name
     */
    std::string getName() const override { 
        return name; 
    }

    /**
     * @brief Returns full SQL column definition.
     *
     * Example:
     * @code
     * age INTEGER NOT NULL
     * @endcode
     *
     * @return SQL definition string
     */
    std::string getDefinition() const override {
        return name + " " + getSQLType<T>() +
               (constraints.empty() ? "" : " " + constraints);
    }

    /**
     * @brief Binds the column value to a SQLite statement.
     *
     * @param stmt Prepared SQLite statement
     * @param index Parameter index (1-based)
     */
    void bindValue(sqlite3_stmt* stmt, int index) const override {
        if constexpr (std::is_same_v<T, int>) {
            sqlite3_bind_int(stmt, index, value);
        }
        else if constexpr (std::is_same_v<T, double>) {
            sqlite3_bind_double(stmt, index, value);
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            sqlite3_bind_text(stmt,
                              index,
                              value.c_str(),
                              -1,
                              SQLITE_TRANSIENT);
        }
    }

    /**
     * @brief Loads value from SQLite query result.
     *
     * @param stmt SQLite statement
     * @param index Column index (0-based)
     */
    void loadFromSQL(sqlite3_stmt* stmt, int index) override {
        if constexpr (std::is_same_v<T, int>) {
            value = sqlite3_column_int(stmt, index);
        }
        else if constexpr (std::is_same_v<T, double>) {
            value = sqlite3_column_double(stmt, index);
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            const unsigned char* text =
                sqlite3_column_text(stmt, index);

            if (text) {
                value = reinterpret_cast<const char*>(text);
            }
            else {
                value.clear();
            }
        }
    }
};

} // namespace fm
