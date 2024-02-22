## Examples

include_directories (${PROJECT_PATH}/Core)
include_directories (${PROJECT_PATH}/Util)

## Bin debugcore
set(SRC_BIN_DCORE     DebugSprCore.cc)
add_executable       (debug_core ${SRC_BIN_DCORE})
set_target_properties(debug_core PROPERTIES OUTPUT_NAME "debugcore")
target_link_libraries(debug_core c sprcore)
add_dependencies     (debug_core libsprcore)

## Bin debugmsg
set(SRC_BIN_DMSG      DebugMsg.cc)
add_executable       (debug_msg ${SRC_BIN_DMSG})
set_target_properties(debug_msg PROPERTIES OUTPUT_NAME "debugmsg")
target_link_libraries(debug_msg c sprcore)
add_dependencies     (debug_msg libsprcore)

## Bin debugtimer
# set(SRC_BIN_DTIMER    DebugTimer.cc)
# add_executable       (debug_timer ${SRC_BIN_DTIMER})
# set_target_properties(debug_timer PROPERTIES OUTPUT_NAME "debugtimer")
# target_link_libraries(debug_timer c sprcore)
# add_dependencies     (debug_timer libsprcore)
