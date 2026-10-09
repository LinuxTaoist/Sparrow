# External (black-box testing with released headers and libraries)
set(TEST_EXTERNAL_PATH      ${PROJECT_PATH}/TestCase/External)

include_directories         (${PROJECT_PATH}/PublicAPIs)
include_directories         (${PROJECT_PATH}/Core)
include_directories         (${PROJECT_PATH}/3rdAdapter)

aux_source_directory        (${TEST_EXTERNAL_PATH}             SRC_TEST_EXTERNAL_PATH)

## Business TestCase External
foreach(module IN LISTS BUSINESS_MODULES)
    set(BUSINESS_TEST_EXTERNAL_PATH ${PROJECT_PATH}/Components/Business/${module}/TestCase/External)
    if(EXISTS ${BUSINESS_TEST_EXTERNAL_PATH})
        file(GLOB BUSINESS_TEST_EXTERNAL_SOURCES
            ${BUSINESS_TEST_EXTERNAL_PATH}/*.cc
            ${BUSINESS_TEST_EXTERNAL_PATH}/*.cpp)
        if(BUSINESS_TEST_EXTERNAL_SOURCES)
            include_directories(${BUSINESS_TEST_EXTERNAL_PATH})
            list(APPEND SRC_TEST_EXTERNAL_PATH ${BUSINESS_TEST_EXTERNAL_SOURCES})
        endif()
    endif()
endforeach()

## Bin test_external
add_executable              (test_external ${SRC_TEST_EXTERNAL_PATH})
target_include_directories  (test_external PRIVATE ${PROJECT_PATH}/3rdParty/googletest/include)
set_target_properties       (test_external PROPERTIES OUTPUT_NAME "test_external")
target_link_libraries       (test_external c LibSprLog LibConfigAPI gtest gtest_main pthread rt dl)
add_dependencies            (test_external LibSprLog LibConfigAPI)
add_test                    (NAME test_external COMMAND test_external)
