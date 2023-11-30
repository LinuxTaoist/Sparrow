## Bin DemoSample1
include_directories (${PROJECT_PATH}/Core)

set(SRC_BIN_DCORE     DebugSprCore.cc)
add_executable       (debug_core ${SRC_BIN_DCORE})
set_target_properties(debug_core PROPERTIES OUTPUT_NAME "DebugSprCore")
target_link_libraries(debug_core c sprcore)
add_dependencies     (debug_core libsprcore)

set(SRC_BIN_SAMPLE1  Sample1.cc)
add_executable       (demo_sample1 ${SRC_BIN_SAMPLE1})
set_target_properties(demo_sample1 PROPERTIES OUTPUT_NAME "DemoSample1")
target_link_libraries(demo_sample1 c sprcore)
add_dependencies     (demo_sample1 libsprcore)