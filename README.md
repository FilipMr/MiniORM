# MiniORM (C++)

A small educational ORM written in C++.
Supports database connection, basic CRUD, simple query building, and SQLite via an abstract DB layer.

---

## Features
- Abstract DB interfaces (`IDbConnection`, `IDbStatement`)
- SQLite implementation (`SqliteConnection`)
- `Entity` + `EntityMapping<T>` for class-to-table mapping
- Generic `Repository<T>` for CRUD
- Fluent `Query<T>` (`where`, `order_by`, `limit`)
- OOP architecture

---

## Build (CMake)

```sh
mkdir build
cd build
cmake ..
make
./main
