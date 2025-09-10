cmake_minimum_required(VERSION 3.10)
project(NetSlangDict VERSION 1.0)

# 定义 C++ 标准，这里使用 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加子目录
add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(common)