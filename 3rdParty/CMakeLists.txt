set(3RD_PATH ${CMAKE_CURRENT_SOURCE_DIR})

execute_process(
    COMMAND cp ${3RD_PATH}/libgo/build/liblibgo.a ${LIBRARY_OUTPUT_PATH}
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
)

