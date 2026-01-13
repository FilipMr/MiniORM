#include <iostream>
#include <string>
#include <sqlite3.h>



//for displaying columns and values
int printCallback(void*, int argc, char** argv, char** colName) {
    for (int i = 0; i < argc; i++) {
        std::cout << colName[i] << "=" << (argv[i] ? argv[i] : "NULL");
        if (i + 1 < argc) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
    return 0;
}

void exec(sqlite3* db, const std::string& sql) {
    char* err = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << (err ? err : "") << std::endl;
        sqlite3_free(err);
    }
}

void query(sqlite3* db, const std::string& sql) {
    char* err = nullptr;
    sqlite3_exec(db, sql.c_str(), printCallback, nullptr, &err);
    sqlite3_free(err);
}

int main() {
    sqlite3* db = nullptr;
    sqlite3_open("example.db", &db);

    // Create table
    exec(db,
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT,"
        "age INTEGER);"
    );

    // Clean previous data
    exec(db, "DELETE FROM users;");

    // CREATE
    exec(db, "INSERT INTO users(name, age) VALUES('Alice', 30);");
    exec(db, "INSERT INTO users(name, age) VALUES('Bob', 25);");

    std::cout << std::endl;
    std::cout << "After insert" << std::endl;
    query(db, "SELECT * FROM users;");

    // UPDATE
    exec(db, "UPDATE users SET age=31 WHERE name='Alice';");

    std::cout << std::endl;
    std::cout << "After update" << std::endl;
    query(db, "SELECT * FROM users;");

    // DELETE
    exec(db, "DELETE FROM users WHERE name='Bob';");

    std::cout << std::endl;
    std::cout << "After delete" << std::endl;
    query(db, "SELECT * FROM users;");

    sqlite3_close(db);
    return 0;
}