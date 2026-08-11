## Components

## Basics
add_subdirectory(Basics)

## Basics
file(GLOB SUB_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/Business/*)

foreach(SUB_DIR ${SUB_DIRS})
  if(EXISTS ${SUB_DIR}/CMakeLists.txt)
    message(STATUS "Building Business: ${SUB_DIR}")
    add_subdirectory(${SUB_DIR})
  endif()
endforeach()
