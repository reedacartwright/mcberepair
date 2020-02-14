cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0074 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")

project(mcberepair VERSION 0.1 LANGUAGES CXX)

find_package(LEVELDB_MCPE)

# GOOGLE disables RTTI for leveldb so we will do the same
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # Disable RTTI.
  string(REGEX REPLACE "/GR" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
else()
  # Disable RTTI.
  string(REGEX REPLACE "-frtti" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()


add_executable(listkeys listkeys.cc)
target_link_libraries(listkeys LEVELDB_MCPE::LEVELDB_MCPE)

add_executable(rmkeys rmkeys.cc)
target_link_libraries(rmkeys LEVELDB_MCPE::LEVELDB_MCPE)

add_executable(dumpkey dumpkey.cc)
target_link_libraries(dumpkey LEVELDB_MCPE::LEVELDB_MCPE)

add_executable(writekey writekey.cc)
target_link_libraries(writekey LEVELDB_MCPE::LEVELDB_MCPE)

add_executable(repair repair.cc)
target_link_libraries(repair LEVELDB_MCPE::LEVELDB_MCPE)

configure_file(mcberepair.sublime-project.in mcberepair.sublime-project @ONLY)
