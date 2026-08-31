# Benchmark
set(TEST_BENCHMARK_PATH      ${PROJECT_PATH}/TestCase/Benchmark)

include_directories         (${PROJECT_PATH}/3rdAdapter)
include_directories         (${PROJECT_PATH}/Core)
include_directories         (${PROJECT_PATH}/Util)
include_directories         (${PROJECT_PATH}/PublicAPIs)
include_directories         (${PROJECT_PATH}/PrivateAPIs/Event)
include_directories         (${PROJECT_PATH}/PrivateAPIs/Binder)
include_directories         (${PROJECT_PATH}/UtilModules/CodecX)
include_directories         (${PROJECT_PATH}/UtilModules/EpollEventHandler)

aux_source_directory        (${PROJECT_PATH}/Core               SRC_TEST_BENCHMARK_PATH)
aux_source_directory        (${TEST_BENCHMARK_PATH}             SRC_TEST_BENCHMARK_PATH)
aux_source_directory        (${PROJECT_PATH}/Util               SRC_TEST_BENCHMARK_PATH)
aux_source_directory        (${PROJECT_PATH}/UtilModules/EpollEventHandler SRC_TEST_BENCHMARK_PATH)
aux_source_directory        (${PROJECT_PATH}/PrivateAPIs/Binder SRC_TEST_BENCHMARK_PATH)
aux_source_directory        (${PROJECT_PATH}/PrivateAPIs/Event  SRC_TEST_BENCHMARK_PATH)

## Bin test_benchmark
add_executable              (test_benchmark ${SRC_TEST_BENCHMARK_PATH})
target_include_directories  (test_benchmark PRIVATE ${PROJECT_PATH}/3rdParty/googletest/include)
set_target_properties       (test_benchmark PROPERTIES OUTPUT_NAME "test_benchmark")
target_link_libraries       (test_benchmark c LibSprLog LibEpollEvent LibConfigAPI gtest gtest_main pthread rt dl)
add_dependencies            (test_benchmark LibSprLog LibEpollEvent LibConfigAPI)
add_test                    (NAME test_benchmark COMMAND test_benchmark)
