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

include("${CMAKE_CURRENT_LIST_DIR}/configure.cmake")

set(GITMETA_BUILT_FROM_GIT)
set(gitmeta_always)

find_package(Git)

set(GITMETA_DIRTY_STR)

# Process Git Hash information
if("$Format:$" STREQUAL "")
  set(GITMETA_HASH       "$Format:%H$")
  set(GITMETA_HASH_SHORT "$Format:%h$")

  option(THE_SOURCE_IS_PATCHED "Set to ON if patches have been applied." OFF)
  if(THE_SOURCE_IS_PATCHED)
    set(GITMETA_DIRTY 1)
  endif()
elseif(GIT_FOUND)
  set(gitmeta_always "ALWAYS")
  ## script code
  set(configure_script_code "
if (IS_DIRECTORY \"${CMAKE_SOURCE_DIR}/.git\")
  set(GITMETA_BUILT_FROM_GIT TRUE)
  execute_process(
    COMMAND           \"${GIT_EXECUTABLE}\"
                      rev-parse
                      HEAD
    WORKING_DIRECTORY \"${CMAKE_SOURCE_DIR}\"
    RESULT_VARIABLE   git_return
    OUTPUT_VARIABLE   GITMETA_HASH)
  execute_process(
    COMMAND           \"${GIT_EXECUTABLE}\"
                      rev-parse
                      --short
                      HEAD
    WORKING_DIRECTORY \"${CMAKE_SOURCE_DIR}\"
    RESULT_VARIABLE   git_return
    OUTPUT_VARIABLE   GITMETA_HASH_SHORT)
  execute_process(
    COMMAND           \"${GIT_EXECUTABLE}\"
                      diff
                      --no-ext-diff
                      --quiet
                      --exit-code
    WORKING_DIRECTORY \"${CMAKE_SOURCE_DIR}\"
    RESULT_VARIABLE   git_return)
  if(git_return)
    set(GITMETA_DIRTY 1)
  endif()
  string(STRIP \"\${GITMETA_HASH}\" GITMETA_HASH)
  string(STRIP \"\${GITMETA_HASH_SHORT}\" GITMETA_HASH_SHORT)
endif()
")
## end script code
else()
  set(GITMETA_HASH       "<unknown>")
  set(GITMETA_HASH_SHORT "<unknown>")
  set(GITMETA_DIRTY      "<unknown>")
endif()

string(APPEND configure_script_code "
  if(GITMETA_DIRTY)
    set(GITMETA_DIRTY_STR \"dirty\")
  endif()
  string(JOIN \"-\" GITMETA_METADATA \"g\${GITMETA_HASH_SHORT}\" \${GITMETA_DIRTY_STR})
  #message(STATUS \"Git Metadata: \${GITMETA_METADATA}\")
")

set(gitmeta_env
  GITMETA_HASH
  GITMETA_HASH_SHORT
  GITMETA_DIRTY
  GITMETA_DIRTY_STR
  GITMETA_METADATA
)

configured_file_content_varonly(gitmeta_content ${gitmeta_env})

add_configured_file(gitmeta.version
  NOTARGET
  CONTENT "${gitmeta_content}"
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/gitmeta.version"
  ${gitmeta_always}
  VARIABLES ${gitmeta_env}
)

set(configure_script_code)
