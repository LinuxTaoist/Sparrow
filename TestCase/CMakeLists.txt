## TestCase
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wno-narrowing")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-narrowing")

enable_testing()

add_subdirectory(External)
add_subdirectory(Internal)
add_subdirectory(Benchmark)