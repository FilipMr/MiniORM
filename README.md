# MiniORM (C++)

A small educational ORM written in C++.
Supports basic CRUD, simple query building, map tables to classes.
Works on SQLite via sglite3 library adding an abstract DB layer.

---

## Features
- Database connection
- Map tables ↔ C++ classes
- Map rows ↔ objects
- Handle CRUD automatically
- OOP architecture

---

## Build (CMake)

```sh
mkdir build
cd build
cmake ..
make
```

## Tests

The file rawsql_test.cpp shows the basic usage of sqlite3

```sh
cd build
./rawsql_test
```

The file orm_test.cpp shows the same case using the MiniORM

```sh
cd build
./orm_test
```