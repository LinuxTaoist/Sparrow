## Business

# 动态加载, 配置文件modules_configs.cmake
foreach(module IN LISTS BUSINESS_MODULES)
    message(STATUS "Add Business Module: ${module}")
    add_subdirectory(${module})
endforeach()
