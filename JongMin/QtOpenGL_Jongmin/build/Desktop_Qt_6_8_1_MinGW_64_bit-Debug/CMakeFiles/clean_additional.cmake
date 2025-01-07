# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\QtOpenGL_Jongmin_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\QtOpenGL_Jongmin_autogen.dir\\ParseCache.txt"
  "QtOpenGL_Jongmin_autogen"
  )
endif()
