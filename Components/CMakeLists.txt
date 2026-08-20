## Components
file(GLOB SUB_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/*)

foreach(SUB_DIR ${SUB_DIRS})
  if(EXISTS ${SUB_DIR}/CMakeLists.txt)
    add_subdirectory(${SUB_DIR})
  endif()
endforeach()
