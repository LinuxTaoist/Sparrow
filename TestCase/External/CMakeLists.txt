# External (black-box testing with released headers and libraries)
set(TEST_EXTERNAL_PATH      ${PROJECT_PATH}/TestCase/External)

include_directories         (${PROJECT_PATH}/PublicAPIs)
include_directories         (${PROJECT_PATH}/Core)
include_directories         (${PROJECT_PATH}/3rdAdapter)

aux_source_directory        (${TEST_EXTERNAL_PATH}             SRC_TEST_EXTERNAL_PATH)

## Bin test_external
add_executable              (test_external ${SRC_TEST_EXTERNAL_PATH})
target_include_directories  (test_external PRIVATE ${PROJECT_PATH}/3rdParty/googletest/include)
set_target_properties       (test_external PROPERTIES OUTPUT_NAME "test_external")
target_link_libraries       (test_external c LibSprLog LibConfigAPI gtest gtest_main pthread rt dl)
add_dependencies            (test_external LibSprLog LibConfigAPI)
