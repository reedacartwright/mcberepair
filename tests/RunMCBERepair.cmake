# Copyright (c) 2020 Reed A. Cartwright <reed@cartwright.ht>
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

function(run_mcberepair test)
  set(top_src "${RunMCBERepair_SOURCE_DIR}")
  set(top_bin "${RunMCBERepair_BINARY_DIR}")
  set(test_args "${ARGN}")
  if(EXISTS ${top_src}/${test}-result.txt)
    file(READ ${top_src}/${test}-result.txt expect_result)
    string(REGEX REPLACE "\n+$" "" expect_result "${expect_result}")
  else()
    set(expect_result 0)
  endif()

  foreach(o out err)
    if(EXISTS ${top_src}/${test}-std${o}.txt)
      file(READ ${top_src}/${test}-std${o}.txt expect_std${o})
      string(REGEX REPLACE "\n+$" "" expect_std${o} "${expect_std${o}}")
    else()
      unset(expect_std${o})
    endif()
  endforeach()
  if (NOT expect_stderr)
    if (NOT RunMCBERepair_DEFAULT_stderr)
      set(RunMCBERepair_DEFAULT_stderr "^$")
    endif()
    set(expect_stderr ${RunMCBERepair_DEFAULT_stderr})
  endif()

  if (NOT RunMCBERepair_TEST_SOURCE_DIR)
    set(RunMCBERepair_TEST_SOURCE_DIR "${top_src}")
  endif()
  if(NOT RunMCBERepair_TEST_BINARY_DIR)
    set(RunMCBERepair_TEST_BINARY_DIR "${top_bin}/${test}-build")
  endif()
  if(NOT RunMCBERepair_TEST_NO_CLEAN)
    file(REMOVE_RECURSE "${RunMCBERepair_TEST_BINARY_DIR}")
  endif()
  file(MAKE_DIRECTORY "${RunMCBERepair_TEST_BINARY_DIR}")
  if(NOT DEFINED RunMCBERepair_TEST_OPTIONS)
    set(RunMCBERepair_TEST_OPTIONS "")
  endif()
  if(RunMCBERepair_TEST_OUTPUT_MERGE)
    set(actual_stderr_var actual_stdout)
    set(actual_stderr "")
  else()
    set(actual_stderr_var actual_stderr)
  endif()
  if(DEFINED RunMCBERepair_TEST_TIMEOUT)
    set(maybe_timeout TIMEOUT ${RunMCBERepair_TEST_TIMEOUT})
  else()
    set(maybe_timeout "")
  endif()
  if(EXISTS ${top_src}/${test}-stdin.txt)
    set(maybe_input_file INPUT_FILE ${top_src}/${test}-stdin.txt)
  else()
    set(maybe_input_file "")
  endif()
  if(NOT RunMCBERepair_TEST_COMMAND)
    set(RunMCBERepair_TEST_COMMAND ${RunMCBERepair_EXE} ${test_args})
  endif()
  if(NOT RunMCBERepair_TEST_COMMAND_WORKING_DIRECTORY)
    set(RunMCBERepair_TEST_COMMAND_WORKING_DIRECTORY "${RunMCBERepair_TEST_BINARY_DIR}")
  endif()
  execute_process(
    COMMAND ${RunMCBERepair_TEST_COMMAND}
    WORKING_DIRECTORY "${RunMCBERepair_TEST_COMMAND_WORKING_DIRECTORY}"
    OUTPUT_VARIABLE actual_stdout
    ERROR_VARIABLE ${actual_stderr_var}
    RESULT_VARIABLE actual_result
    ENCODING UTF8
    ${maybe_timeout}
    ${maybe_input_file}
    )
  set(msg "")
  if(NOT "${actual_result}" MATCHES "${expect_result}")
    string(APPEND msg "Result is [${actual_result}], not [${expect_result}].\n")
  endif()

  foreach(o out err)
    string(REGEX REPLACE "\r\n" "\n" actual_std${o} "${actual_std${o}}")
    string(REGEX REPLACE "\n+$" "" actual_std${o} "${actual_std${o}}")
    set(expect_${o} "")
    if(DEFINED expect_std${o})
      if(NOT "${actual_std${o}}" MATCHES "${expect_std${o}}")
        string(REGEX REPLACE "\n" "\n expect-${o}> " expect_${o}
          " expect-${o}> ${expect_std${o}}")
        set(expect_${o} "Expected std${o} to match:\n${expect_${o}}\n")
        string(APPEND msg "std${o} does not match that expected.\n")
      endif()
    endif()
  endforeach()
  unset(RunMCBERepair_TEST_FAILED)
  include(${top_src}/${test}-check.cmake OPTIONAL)

  if(RunMCBERepair_TEST_FAILED)
    set(msg "${RunMCBERepair_TEST_FAILED}\n${msg}")
  endif()
  if(msg)
    string(REPLACE ";" "\" \"" command "\"${RunMCBERepair_TEST_COMMAND}\"")
    string(APPEND msg "Command was:\n command> ${command}\n")
  endif()
  if(msg)
    string(REGEX REPLACE "\n" "\n actual-out> " actual_out " actual-out> ${actual_stdout}")
    string(REGEX REPLACE "\n" "\n actual-err> " actual_err " actual-err> ${actual_stderr}")
    message(SEND_ERROR "${test} - FAILED:\n"
      "${msg}"
      "${expect_out}"
      "Actual stdout:\n${actual_out}\n"
      "${expect_err}"
      "Actual stderr:\n${actual_err}\n"
      )
  else()
    message(STATUS "${test} - PASSED")
  endif()
endfunction()

function(extract_world world_dir mcworld)
  file(REMOVE_RECURSE "${world_dir}")
  file(MAKE_DIRECTORY "${world_dir}")  
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E tar x "${mcworld}"
    WORKING_DIRECTORY "${world_dir}"
  )
endfunction()
