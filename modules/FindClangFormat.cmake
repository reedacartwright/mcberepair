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

set(clang_format_names clang-format)

find_program(CLANG_FORMAT_EXECUTABLE
  NAMES ${clang_format_names}
  DOC "clang-format command line client"
)

unset(clang_format_names)

mark_as_advanced(CLANG_FORMAT_EXECUTABLE)

if(CLANG_FORMAT_EXECUTABLE)
  execute_process(COMMAND "${CLANG_FORMAT_EXECUTABLE}" -version
                  OUTPUT_VARIABLE clang_format_version_str
                  ERROR_QUIET
                  OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(clang_format_version_str MATCHES "^clang-format version ([0-9][0-9.]+)")
    set(CLANG_FORMAT_VERSION "${CMAKE_MATCH_1}")
  endif()

  unset(clang_format_version_str)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ClangFormat
  REQUIRED_VARS CLANG_FORMAT_EXECUTABLE
  VERSION_VAR CLANG_FORMAT_VERSION
)
