#include <iostream>
#include <string>
#include <sqlite3.h>

void exec(sqlite3* db, const std::string& sql) {
    char* err = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << (err ? err : "") << std::endl;
        sqlite3_free(err);
    }
}

int printCallback(void*, int argc, char** argv, char** colName) {
    for (int i = 0; i < argc; i++) {
        std::cout << colName[i] << "="
                  << (argv[i] ? argv[i] : "NULL");
        if (i + 1 < argc) std::cout << ", ";
    }
    std::cout << std::endl;
    return 0;
}

void query(sqlite3* db, const std::string& sql) {
    char* err = nullptr;
    sqlite3_exec(db, sql.c_str(), printCallback, nullptr, &err);
    sqlite3_free(err);
}

int main() {
    std::cout << "Not ready yet" << std::endl;

    return 0;
}