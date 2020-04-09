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

set(gcovr_html "coverage.html")
set(gcovr_xml  "cobertura.xml")
set(gcovr_exclude
  "${CMAKE_SOURCE_DIR}/leveldb-mcpe"
)

find_package(Gcovr)

# if gcovr was found add targets
#   make coverage_html
#   make coverage_xml
if(Gcovr_FOUND)
  foreach(pat IN LISTS gcovr_exclude)
    LIST(APPEND gcovr_exclude_ --exclude "${pat}")
  endforeach()

  add_custom_command(
    OUTPUT "${gcovr_html}" "${gcovr_html}.noexist"
    COMMAND "${GCOVR_EXECUTABLE}"
      --root "${CMAKE_SOURCE_DIR}"
      --delete
      --exclude-unreachable-branches
      ${gcovr_exclude_}
      --html-details
      --output "${gcovr_html}"
  )
  add_custom_target(coverage_html
    DEPENDS "${gcovr_html}"
  )

  add_custom_command(
    OUTPUT "${gcovr_xml}" "${gcovr_xml}.noexist"
    COMMAND "${GCOVR_EXECUTABLE}"
      --root "${CMAKE_SOURCE_DIR}"
      --delete
      --exclude-unreachable-branches
      ${gcovr_exclude_}
      --xml
      --output "${gcovr_xml}"
  )

  add_custom_target(coverage_xml
    DEPENDS "${gcovr_xml}"
  )
endif()

# Create a script to download codecov.bash script
file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/download_codecov.cmake" CONTENT "
file(DOWNLOAD https://codecov.io/bash \"${CMAKE_CURRENT_BINARY_DIR}/codecov.bash\")
")

# Add support for codecov.io
#   make upload_codecov
add_custom_command(
  OUTPUT codecov.bash
  COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/download_codecov.cmake"
)

add_custom_target(upload_codecov
  DEPENDS codecov.bash
  COMMAND rm -f "${gcovr_xml}"
  COMMAND bash "${CMAKE_CURRENT_BINARY_DIR}/codecov.bash"
)
