# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\VibePay_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\VibePay_autogen.dir\\ParseCache.txt"
  "VibePay_autogen"
  )
endif()
