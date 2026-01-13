#pragma once
#include <string>
#include <sqlite3.h>

namespace fm {

/**
 * @brief Abstract interface representing a database column.
 *
 * This interface provides a common API for all column types
 * used by the ORM system. Concrete implementations (such as
 * fm::Column<T>) handle:
 *
 * - SQL definition generation
 * - Value binding to SQLite statements
 * - Loading values from query results
 *
 * This allows the Model class to treat all columns uniformly.
 */
class IColumn {
public:
    /**
     * @brief Returns the column name.
     *
     * @return Column name as string.
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Returns the full SQL column definition.
     *
     * Example:
     * @code
     * id INTEGER PRIMARY KEY
     * @endcode
     *
     * @return SQL column definition.
     */
    virtual std::string getDefinition() const = 0;

    /**
     * @brief Binds the column value to a SQLite prepared statement.
     *
     * @param stmt SQLite prepared statement.
     * @param index Parameter index (1-based).
     */
    virtual void bindValue(sqlite3_stmt* stmt, int index) const = 0;

    /**
     * @brief Loads the column value from a SQLite query result.
     *
     * @param stmt SQLite statement.
     * @param index Column index (0-based).
     */
    virtual void loadFromSQL(sqlite3_stmt* stmt, int index) = 0;

    /**
     * @brief Virtual destructor for safe polymorphic deletion.
     */
    virtual ~IColumn() = default;
};

} // namespace fm
