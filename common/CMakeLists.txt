cmake_minimum_required(VERSION 3.10)

# 添加子模块
add_subdirectory(httplib)
add_subdirectory(json)

set(COMMON_SOURCES
    protocol.cpp
)

# 构建共享库
add_library(common STATIC ${COMMON_SOURCES})

# 确保头文件可以被外部引用
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 链接 httplib 库
target_link_libraries(common PRIVATE httplib::httplib nlohmann_json::nlohmann_json)