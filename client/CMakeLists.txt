cmake_minimum_required(VERSION 3.10)
project(ClientApp)

set(CMAKE_CXX_STANDARD 17)

# 查找 ftxui
find_package(ftxui REQUIRED)

# 显式列出所有源文件，避免使用 glob
set(CLIENT_SOURCES
    main.cpp
)

# 添加可执行文件
add_executable(client_app ${CLIENT_SOURCES})

target_link_libraries(client_app PRIVATE common)

target_link_libraries(client_app
    PRIVATE
        ftxui::screen
        ftxui::dom
        ftxui::component
        httplib::httplib
)
