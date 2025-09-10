cmake_minimum_required(VERSION 3.10)
project(ClientApp)

# 找到源文件
file(GLOB SERVER_SOURCES "*.cpp" "*.h")

# 添加可执行文件
add_executable(server_app ${SERVER_SOURCES})

# 链接共享的库
target_link_libraries(server_app PRIVATE common)