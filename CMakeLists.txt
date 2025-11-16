cmake_minimum_required(VERSION 4.0)
project(ALLS)

set(CMAKE_CXX_STANDARD 26)
find_package(raylib CONFIG REQUIRED)
find_package(unofficial-enet CONFIG REQUIRED)
add_executable(ALLS main.cpp
        network/Server.cpp
        network/Client.cpp
        network/Client.h)
target_link_libraries(ALLS PRIVATE raylib)
target_link_libraries(ALLS PRIVATE unofficial::enet::enet winmm ws2_32)