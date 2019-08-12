cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0074 NEW)

set(CMAKE_CXX_STANDARD 14)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")

project(mcberepair VERSION 0.1 LANGUAGES CXX)

find_package(LEVELDB_MCPE)

add_executable(listkeys listkeys.cc)
target_link_libraries(listkeys LEVELDB_MCPE::LEVELDB_MCPE)

add_executable(rmkeys rmkeys.cc)
target_link_libraries(rmkeys LEVELDB_MCPE::LEVELDB_MCPE)
