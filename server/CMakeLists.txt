cmake_minimum_required(VERSION 3.10)
project(Serverapp)

set(SERVER_SOURCES
    main.cpp
)

# 添加可执行文件
add_executable(server_app ${SERVER_SOURCES})

# 链接共享的库
target_link_libraries(server_app PRIVATE common)

target_link_libraries(server_app
    PRIVATE
        httplib::httplib
)