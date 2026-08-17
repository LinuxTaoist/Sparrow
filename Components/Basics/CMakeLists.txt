# Basics

## Project configures path
set (GENERAL_CONFIGURE_PATH ${PROJECT_PATH}/Configs/General)
set (VENDOR_CONFIGURE_PATH  ${PROJECT_PATH}/Platform/${PROJECT_PLATFORM}/Configs)

## Project general configures
file(COPY ${GENERAL_CONFIGURE_PATH}/default.prop        DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
file(COPY ${GENERAL_CONFIGURE_PATH}/config_manager.conf DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
file(COPY ${GENERAL_CONFIGURE_PATH}/env_setup.sh        DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
file(COPY ${GENERAL_CONFIGURE_PATH}/system.prop         DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)

## Project vendor configures
file(COPY ${VENDOR_CONFIGURE_PATH}/vendor.prop          DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
file(COPY ${VENDOR_CONFIGURE_PATH}/sprlog.conf          DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)

if (EXISTS ${VENDOR_CONFIGURE_PATH}/init.conf)
    file(COPY ${VENDOR_CONFIGURE_PATH}/init.conf        DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
else()
    file(COPY ${GENERAL_CONFIGURE_PATH}/init.conf        DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
endif()

# 动态加载, 配置文件modules_configs.cmake
foreach(module IN LISTS BASIC_MODULES)
    message(STATUS "Add Basic Module: ${module}")
    add_subdirectory(${module})
endforeach()
