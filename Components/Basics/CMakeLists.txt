# Basics

## Project configures path
set (GENERAL_CONFIGURE_PATH ${PROJECT_PATH}/Configs/General)
set (VENDOR_CONFIGURE_PATH  ${PROJECT_PATH}/Platform/${PROJECT_PLATFORM}/Configs)

## Project general configures
file(COPY ${GENERAL_CONFIGURE_PATH}/env_setup.sh        DESTINATION ${EXECUTABLE_OUTPUT_PATH})
file(COPY ${GENERAL_CONFIGURE_PATH}/default.prop        DESTINATION ${ETC_OUTPUT_PATH})
file(COPY ${GENERAL_CONFIGURE_PATH}/system.prop         DESTINATION ${ETC_OUTPUT_PATH})

## Platform override configures
set (PLATFORM_OVERRIDE_CONFIGURES
    vendor.prop
    init.conf
    sprlog.conf
    config_manager.conf
)

foreach(configure IN LISTS PLATFORM_OVERRIDE_CONFIGURES)
    if (EXISTS ${VENDOR_CONFIGURE_PATH}/${configure})
        file(COPY ${VENDOR_CONFIGURE_PATH}/${configure}  DESTINATION ${ETC_OUTPUT_PATH})
    elseif (EXISTS ${GENERAL_CONFIGURE_PATH}/${configure})
        file(COPY ${GENERAL_CONFIGURE_PATH}/${configure} DESTINATION ${ETC_OUTPUT_PATH})
    endif()
endforeach()

# 动态加载, 配置文件modules_configs.cmake
foreach(module IN LISTS BASIC_MODULES)
    message(STATUS "Add Basic Module: ${module}")
    add_subdirectory(${module})
endforeach()
