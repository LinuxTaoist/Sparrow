## Bin test_internal
set(TEST_INTERNAL_PATH      ${PROJECT_PATH}/Test/Internal)

include_directories         (${PROJECT_PATH}/Util)

aux_source_directory        (${PROJECT_PATH}/Util           SRC_TEST_INTERNAL_PATH)
aux_source_directory        (${TEST_INTERNAL_PATH}/Util     SRC_TEST_INTERNAL_PATH)
add_executable              (test_internal ${SRC_TEST_INTERNAL_PATH})
target_include_directories  (test_internal PRIVATE ${PROJECT_PATH}/3rdParty/googletest/include)
set_target_properties       (test_internal PROPERTIES OUTPUT_NAME "test_internal")
target_link_libraries       (test_internal c gtest gtest_main)
add_dependencies            (test_internal gtest gtest_main)