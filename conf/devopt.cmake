# This CMake File defines several useful developer options

SET(DEVOPT_ENABLE_GPERFTOOLS OFF CACHE BOOL "Enable profiling with gperftools.")
SET(DEVOPT_ENABLE_COVERAGE_REPORT OFF CACHE BOOL "Enable code coverage reporting.")
SET(DEVOPT_EXCLUDE_PRETEST_FROM_ALL OFF CACHE BOOL "Do not build the pretest target by default.")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# copied from CMAKE source code
set(header_regex "\\.(h|hh|h\\+\\+|hm|hpp|hxx|in|txx|inl)$")

SET(devopt_LIBRARIES)

if(DEVOPT_ENABLE_GPERFTOOLS)
  find_package(Gperftools COMPONENTS profiler)
  if(GPERFTOOLS_FOUND)
    message(STATUS "DEVOPT: Profiling with gperftools enabled. Use CPUPROFILE environmental variable to turn on profiling and specify output file.")
    set(devopt_LIBRARIES ${devopt_LIBRARIES} GPERFTOOLS::GPERFTOOLS)
  else()
    message(FATAL_ERROR "Gperftools was not found. Please disable the flag DEVOPT_ENABLE_GPERFTOOLS and try again.")
  endif()
endif()

#####################################################################
# COVERAGE REPORT

add_library(devopt_coverage INTERFACE)

if(DEVOPT_ENABLE_COVERAGE_REPORT AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  # Add required flags (GCC & LLVM/Clang)
  target_compile_options(devopt_coverage INTERFACE
    -O0        # no optimization
    -g         # generate debug info
    --coverage # sets all required flags
  )
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
    target_link_options(devopt_coverage INTERFACE --coverage)
  else()
    target_link_libraries(devopt_coverage INTERFACE --coverage)
  endif()

  # setup targets for generating reports
  add_subdirectory("${CMAKE_SOURCE_DIR}/coverage")
endif()

#####################################################################
# CLANG FORMAT

# Setup code formatting if possible
find_package(ClangFormat)

function(clang_format_target target) 
  get_target_property(target_sources "${target}" SOURCES)
  clang_format_files("${target}" ${target_sources})
endfunction()

function(clang_format_files name)
  if(NOT ClangFormat_FOUND)
    return() # a NOOP 
  endif()
  
  foreach(source ${ARGN})
    get_filename_component(source "${source}" ABSOLUTE)
    list(APPEND sources "${source}")
  endforeach()

  add_custom_target(format_${name}
    COMMAND "${CLANG_FORMAT_EXECUTABLE}"
      -style=file -i ${sources}
    COMMENT
      "Formating sources of ${name} ..."
  )
  add_dependencies(format_${name} configure)

  if(TARGET format)
    add_dependencies(format format_${name})
  else()
    add_custom_target(format DEPENDS format_${name})
  endif()

  add_custom_target(check_format_${name}
    COMMAND "${CLANG_FORMAT_EXECUTABLE}"
      -style=file -output-replacements-xml
      ${sources} > ${CMAKE_BINARY_DIR}/check_format_${name}.xml
    COMMAND ! grep -q "'\\breplacement\\b'" ${CMAKE_BINARY_DIR}/check_format_${name}.xml
    BYPRODUCTS 
      ${CMAKE_BINARY_DIR}/check_format_${name}.xml
    COMMENT
      "Checking format of sources of ${name} ..."
  )
  add_dependencies(check_format_${name} configure)

  if(TARGET check_format)
    add_dependencies(check_format check_format_${name})
  else()
    add_custom_target(check_format DEPENDS check_format_${name})
  endif()

endfunction()

#####################################################################
# CPPCHECK

find_package(Cppcheck)

if(Cppcheck_FOUND AND CMAKE_EXPORT_COMPILE_COMMANDS)

add_custom_target(cppcheck
  COMMAND "${CPPCHECK_EXECUTABLE}"
    "--project=${CMAKE_BINARY_DIR}/compile_commands.json"
    "--suppressions-list=${CMAKE_CURRENT_LIST_DIR}/cppcheck_suppressions.txt"
    --enable=all
    --inline-suppr
    --quiet
  COMMENT "Looking for programming errors with Cppcheck ..."
)
add_dependencies(cppcheck configure)

add_custom_target(check_cppcheck
  COMMAND "${CPPCHECK_EXECUTABLE}"
    "--project=${CMAKE_BINARY_DIR}/compile_commands.json"
    "--suppressions-list=${CMAKE_CURRENT_LIST_DIR}/cppcheck_suppressions.txt"
    --enable=all
    --inline-suppr
    --error-exitcode=2
    --quiet
    --xml
    "--output-file=${CMAKE_BINARY_DIR}/cppcheck_results.xml"
  BYPRODUCTS
    "${CMAKE_BINARY_DIR}/cppcheck_results.xml"
  COMMENT "Checking if source code passes Cppcheck ..."
)
add_dependencies(check_cppcheck configure)

endif()

#####################################################################
# CLANG-TIDY

find_package(ClangTidy)

function(clang_tidy_target target) 
  get_target_property(target_sources "${target}" SOURCES)
  clang_tidy_files("${target}" ${target_sources})
endfunction()

function(clang_tidy_files name)
  if(NOT ClangTidy_FOUND)
    return() # a NOOP 
  endif()
  
  foreach(source ${ARGN})
    if(NOT source MATCHES "${header_regex}")
      get_filename_component(source "${source}" ABSOLUTE)
      list(APPEND sources "${source}")
    endif()
  endforeach()

  add_custom_target(tidy_${name}
    COMMAND "${CLANG_TIDY_EXECUTABLE}"
      "-p=${CMAKE_BINARY_DIR}"
      ${sources}
    COMMENT "Looking for programming errors with ClangTidy ..."
  )
  add_dependencies(tidy_${name} configure)
  if(TARGET tidy)
    add_dependencies(tidy tidy_${name})
  else()
    add_custom_target(tidy DEPENDS tidy_${name})
  endif()

  add_custom_target(check_tidy_${name}
    COMMAND "${CLANG_TIDY_EXECUTABLE}"
      "-p=${CMAKE_BINARY_DIR}"
      -quiet
      "-export-fixes=${CMAKE_BINARY_DIR}/check_tidy_${name}.fixes.yml"
      ${sources}
      > "${CMAKE_BINARY_DIR}/check_tidy_${name}.txt"
    COMMAND test ! -s "${CMAKE_BINARY_DIR}/check_tidy_${name}.txt"
    BYPRODUCTS
      "${CMAKE_BINARY_DIR}/check_tidy_${name}.fixes.yml"
      "${CMAKE_BINARY_DIR}/check_tidy_${name}.txt"
    COMMENT "Checking if source code passes ClangTidy ..."    
  )
  add_dependencies(check_tidy_${name} configure)
  if(TARGET check_tidy)
    add_dependencies(check_tidy check_tidy_${name})
  else()
    add_custom_target(check_tidy DEPENDS check_tidy_${name})
  endif()

endfunction()

#####################################################################
# Check All Target

add_custom_target(check_test COMMAND "${CMAKE_CTEST_COMMAND}" DEPENDS pretest)
add_custom_target(check_all DEPENDS check_test check_format check_tidy check_cppcheck)

