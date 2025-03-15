set(3RD_PATH ${CMAKE_CURRENT_SOURCE_DIR})

#execute_process(
#    COMMAND mkdir -p ${LIBRARY_OUTPUT_PATH}
#    COMMAND cp ${3RD_PATH}/libgo/lib/${PROJECT_PLATFORM}/liblibgo.a ${LIBRARY_OUTPUT_PATH}/liblibgo.a
#    COMMAND cp ${3RD_PATH}/sqlite/lib/${PROJECT_PLATFORM}/libsqlite3.a ${LIBRARY_OUTPUT_PATH}/libsqlite3.a
#    RESULT_VARIABLE result
#    OUTPUT_VARIABLE output
#)

if(BUILD_GTEST)
    if(NOT EXISTS ${3RD_PATH}/googletest/googletest)
        message(STATUS "tar -zxf googletest.tar.gz")

        # 解压文件
        execute_process(
            COMMAND tar -zxf ${3RD_PATH}/googletest/googletest.tar.gz -C ${3RD_PATH}/googletest
            RESULT_VARIABLE extract_result
            OUTPUT_VARIABLE extract_output
        )
    endif()

    add_subdirectory(${3RD_PATH}/googletest/googletest)
endif()
