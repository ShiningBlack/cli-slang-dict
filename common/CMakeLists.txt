cmake_minimum_required(VERSION 3.10)
project(ClientApp)

# 找到源文件
file(GLOB COMMON_SOURCES "*.cpp" "*.h")

# 构建共享库
add_library(common STATIC ${COMMON_SOURCES})

# 确保头文件可以被外部引用
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})