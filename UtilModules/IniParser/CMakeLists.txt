# IniParser

# Build libiniparser.so
aux_source_directory    (${CMAKE_CURRENT_SOURCE_DIR} SRC_INI_PARSER_PATH)

add_library             (LibIniParser SHARED ${SRC_INI_PARSER_PATH})
set_target_properties   (LibIniParser PROPERTIES OUTPUT_NAME "iniparser")
set_target_properties   (LibIniParser PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries   (LibIniParser stdc++ rt dl)
