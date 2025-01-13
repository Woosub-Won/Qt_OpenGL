# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\chapter2_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\chapter2_autogen.dir\\ParseCache.txt"
  "chapter2_autogen"
  )
endif()
