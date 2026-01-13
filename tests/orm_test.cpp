#include <iostream>
#include <string>
#include "../src/orm.hpp"

class User : public fm::Model {
public:
    fm::Column<int> id{this, "id", "PRIMARY KEY AUTOINCREMENT"};
    fm::Column<std::string> username{this, "username", "NOT NULL"};
    fm::Column<int> age{this, "age"};

    User() : fm::Model("users") {}
};

int main() {
    sqlite3* db;
    sqlite3_open("test.db", &db);
    
    User u;
    u.create_table(db);
    
    sqlite3_close(db);
    return 0;

    return 0;
}