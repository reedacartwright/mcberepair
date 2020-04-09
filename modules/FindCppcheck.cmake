# Copyright (c) 2019 Reed A. Cartwright <reed@cartwright.ht>
# 
# This file is part of the Ultimate Source Code Project.
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

set(cppcheck_names cppcheck)

find_program(CPPCHECK_EXECUTABLE
  NAMES ${cppcheck_names}
  DOC "cppcheck command line client"
)

unset(cppcheck_names)

mark_as_advanced(CPPCHECK_EXECUTABLE)

if(CPPCHECK_EXECUTABLE)
  execute_process(COMMAND "${CPPCHECK_EXECUTABLE}" --version
                  OUTPUT_VARIABLE cppcheck_version_str
                  ERROR_QUIET
                  OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(cppcheck_version_str MATCHES "^Cppcheck ([0-9][0-9.]+)")
    set(CPPCHECK_VERSION "${CMAKE_MATCH_1}")
  endif()

  unset(cppcheck_version_str)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Cppcheck
  REQUIRED_VARS CPPCHECK_EXECUTABLE
  VERSION_VAR CPPCHECK_VERSION
)
