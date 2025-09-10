cmake_minimum_required(VERSION 3.10)
project(ClientApp)

set(CMAKE_CXX_STANDARD 17)

# 查找 ftxui
find_package(ftxui REQUIRED)

file(GLOB CLIENT_SOURCES "*.cpp" "*.h")

add_executable(client_app ${CLIENT_SOURCES})

# 链接需要的模块
target_link_libraries(client_app
    PRIVATE
        ftxui::screen
        ftxui::dom
        ftxui::component
)
