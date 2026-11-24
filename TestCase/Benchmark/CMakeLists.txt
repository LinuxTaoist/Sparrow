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
include_directories         (${PROJECT_PATH}/UtilModules/Heartbeat)

aux_source_directory        (${PROJECT_PATH}/Core               SRC_TEST_BENCHMARK_PATH)
aux_source_directory        (${TEST_BENCHMARK_PATH}             SRC_TEST_BENCHMARK_PATH)
aux_source_directory        (${PROJECT_PATH}/Util               SRC_TEST_BENCHMARK_PATH)
aux_source_directory        (${PROJECT_PATH}/UtilModules/EpollEventHandler SRC_TEST_BENCHMARK_PATH)
aux_source_directory        (${PROJECT_PATH}/PrivateAPIs/Binder SRC_TEST_BENCHMARK_PATH)
aux_source_directory        (${PROJECT_PATH}/PrivateAPIs/Event  SRC_TEST_BENCHMARK_PATH)

## Bin test_benchmark
add_executable              (BinTestBenchmark ${SRC_TEST_BENCHMARK_PATH})
target_include_directories  (BinTestBenchmark PRIVATE ${PROJECT_PATH}/3rdParty/googletest/include)
set_target_properties       (BinTestBenchmark PROPERTIES OUTPUT_NAME "test_benchmark")
target_link_libraries       (BinTestBenchmark c LibSprLog LibEpollEvent LibConfigAPI LibHeartbeat gtest gtest_main pthread rt dl)
add_dependencies            (BinTestBenchmark LibSprLog LibEpollEvent LibConfigAPI LibHeartbeat)
add_test                    (NAME test_benchmark COMMAND BinTestBenchmark)
