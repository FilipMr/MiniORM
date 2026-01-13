/*
Important note!
ORM is a very big topic, here i have just build the very basic abstraction level of handling db via object-related-mapping.
There are inconsistencies.
I did not have a recourses to create a migration mechanism yet,
so when we use users.create_table() it kind of tells SQLite to make sure a place exists(the table that we defined as a class User in this case) to store those records.

I did not implemet select_all and delete_all we can only do those by ids, so for this show case i have used raw sql to show the data base
*/

#include <iostream>
#include <string>
#include <sqlite3.h>
#include "../src/MiniOrm.hpp"

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

void query(sqlite3* db, const std::string& sql) {
    char* err = nullptr;
    sqlite3_exec(db, sql.c_str(), printCallback, nullptr, &err);
    if (err) {
        std::cout << "Query error: " << err << std::endl;
        sqlite3_free(err);
    }
}

//here we define how the table actually looks
class User : public fm::Model {
public:
    //the pattern is the following
    //we create object field representing our columns with our desirable type and name
    fm::Column<int> id{this, "id", "PRIMARY KEY AUTOINCREMENT"};
    fm::Column<std::string> name{this, "name", "TEXT"};
    fm::Column<int> age{this, "age", "INTEGER"};

    //here in the constructor we are passing the our table's name
    User() : fm::Model("users") {}
};

int main() {
    sqlite3* db = nullptr;
    sqlite3_open("example_orm.db", &db);

    //as i dont have a migration system i use a temporary object to create a table for better visualisation
    User users;

    users.create_table(db);

    //prepare clear databse
    sqlite3_exec(db, "DELETE FROM users;", nullptr, nullptr, nullptr);

    User u1;

    //creating users
    u1.id = 1; 
    u1.name = "Alice";
    u1.age = 30;

    User u2;

    u2.id = 2;
    u2.name = "Bob";
    u2.age = 25;

    //executing
    u1.save(db);
    u2.save(db);

    std::cout << std::endl;
    std::cout << "After insert" << std::endl;
    query(db, "SELECT * FROM users;");

    //to update we check if record exists, if so only then we make our changes and update them
    if (u1.find(db, 1)) {
        u1.age = 31;
        u1.update(db);
    }

    std::cout << std::endl;
    std::cout << "After update" << std::endl;
    query(db, "SELECT * FROM users;");

    //to delete we check if record exists and then we can delete by id
    if (u2.find(db, 2)) {
        u2.remove(db);
    }

    std::cout << std::endl;
    std::cout << "After delete" << std::endl;
    query(db, "SELECT * FROM users;");

    sqlite3_close(db);
    return 0;
}