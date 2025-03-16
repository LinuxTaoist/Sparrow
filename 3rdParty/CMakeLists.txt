set(3RD_PATH ${CMAKE_CURRENT_SOURCE_DIR})

#execute_process(
#    COMMAND mkdir -p ${LIBRARY_OUTPUT_PATH}
#    COMMAND cp ${3RD_PATH}/libgo/lib/${PROJECT_PLATFORM}/liblibgo.a ${LIBRARY_OUTPUT_PATH}/liblibgo.a
#    COMMAND cp ${3RD_PATH}/sqlite/lib/${PROJECT_PLATFORM}/libsqlite3.a ${LIBRARY_OUTPUT_PATH}/libsqlite3.a
#    RESULT_VARIABLE result
#    OUTPUT_VARIABLE output
#)

if(BUILD_GTEST)
    execute_process(
        COMMAND mkdir -p ${LIBRARY_OUTPUT_PATH}
        COMMAND cp -rf ${3RD_PATH}/googletest/lib/${PROJECT_PLATFORM}/libgtest.a ${LIBRARY_OUTPUT_PATH}/
        COMMAND cp -rf ${3RD_PATH}/googletest/lib/${PROJECT_PLATFORM}/libgtest_main.a ${LIBRARY_OUTPUT_PATH}/
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
    )
endif()
