cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0074 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")

project(mcberepair VERSION 0.1 LANGUAGES CXX)

option(LEVELDB_BUILD_TESTS OFF)
add_subdirectory(leveldb-mcpe)

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

foreach(NAME listkeys rmkeys dumpkey writekey repair copy)
	add_executable(${NAME} ${NAME}.cc)
	target_link_libraries(${NAME} leveldb)
endforeach()

configure_file(mcberepair.sublime-project.in mcberepair.sublime-project @ONLY)
