set(3RD_PATH ${CMAKE_CURRENT_SOURCE_DIR})

execute_process(
    COMMAND mkdir -p ${LIBRARY_OUTPUT_PATH}
    COMMAND cp ${3RD_PATH}/libgo/lib/${PROJECT_PLATFORM}/liblibgo.a ${LIBRARY_OUTPUT_PATH}/liblibgo.a
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
)

