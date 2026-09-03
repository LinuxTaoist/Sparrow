# Internal
set(TEST_INTERNAL_PATH      ${PROJECT_PATH}/TestCase/Internal)

include_directories         (${PROJECT_PATH}/3rdAdapter)
include_directories         (${PROJECT_PATH}/Core)
include_directories         (${PROJECT_PATH}/PublicAPIs)
include_directories         (${PROJECT_PATH}/Util)
include_directories         (${PROJECT_PATH}/PrivateAPIs/Event)
include_directories         (${PROJECT_PATH}/PrivateAPIs/Binder)
include_directories         (${PROJECT_PATH}/UtilModules/CodecX)
include_directories         (${PROJECT_PATH}/UtilModules/EpollEventHandler)
include_directories         (${TEST_INTERNAL_PATH}/Components)

aux_source_directory        (${PROJECT_PATH}/Core               SRC_TEST_INTERNAL_PATH)
aux_source_directory        (${TEST_INTERNAL_PATH}/Core         SRC_TEST_INTERNAL_PATH)
aux_source_directory        (${PROJECT_PATH}/Util               SRC_TEST_INTERNAL_PATH)
aux_source_directory        (${TEST_INTERNAL_PATH}/Util         SRC_TEST_INTERNAL_PATH)
aux_source_directory        (${TEST_INTERNAL_PATH}/3rdAdapter   SRC_TEST_INTERNAL_PATH)
aux_source_directory        (${TEST_INTERNAL_PATH}/Components   SRC_TEST_INTERNAL_PATH)
aux_source_directory        (${TEST_INTERNAL_PATH}/UtilModules  SRC_TEST_INTERNAL_PATH)

## Bin test_internal
add_executable              (test_internal ${SRC_TEST_INTERNAL_PATH})
target_include_directories  (test_internal PRIVATE ${PROJECT_PATH}/3rdParty/googletest/include)
set_target_properties       (test_internal PROPERTIES OUTPUT_NAME "test_internal")
target_link_libraries       (test_internal c LibSprLog LibEpollEvent LibCodecX gtest gtest_main pthread rt dl)
add_dependencies            (test_internal LibSprLog LibEpollEvent LibCodecX)
add_test                    (NAME test_internal COMMAND test_internal)
