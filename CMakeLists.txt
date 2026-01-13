cmake_minimum_required(VERSION 3.16)
project(SqliteExecDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(orm src/orm.cpp)
add_executable(rawsql_test tests/rawsql_test.cpp)
add_executable(orm_test tests/orm_test.cpp)

target_link_libraries(orm PRIVATE sqlite3)
target_link_libraries(rawsql_test PRIVATE sqlite3)
target_link_libraries(orm_test PRIVATE sqlite3)