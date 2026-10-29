set(3RD_PATH ${CMAKE_CURRENT_SOURCE_DIR})

execute_process(
    COMMAND mkdir -p ${LIBRARY_OUTPUT_PATH}
    COMMAND cp -rf ${3RD_PATH}/sqlite/lib/${PROJECT_PLATFORM}/libsqlite3.a ${LIBRARY_OUTPUT_PATH}/libsqlite3.a
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
)

if(BUILD_TESTCASE)
    execute_process(
        COMMAND mkdir -p ${LIBRARY_OUTPUT_PATH}
        COMMAND cp -rf ${3RD_PATH}/googletest/lib/${PROJECT_PLATFORM}/libgtest.a ${LIBRARY_OUTPUT_PATH}/
        COMMAND cp -rf ${3RD_PATH}/googletest/lib/${PROJECT_PLATFORM}/libgtest_main.a ${LIBRARY_OUTPUT_PATH}/
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
    )
endif()

if(BUILD_CURL)
    execute_process(
        COMMAND mkdir -p ${LIBRARY_OUTPUT_PATH} ${EXECUTABLE_OUTPUT_PATH}
        COMMAND cp -rf ${3RD_PATH}/curl/lib/${PROJECT_PLATFORM}/libcurl.a ${LIBRARY_OUTPUT_PATH}/
        COMMAND cp -rf ${3RD_PATH}/curl/bin/${PROJECT_PLATFORM}/curl ${EXECUTABLE_OUTPUT_PATH}/curl
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
    )
endif()

