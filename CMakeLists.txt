cmake_minimum_required(VERSION 4.0)
project(ALLS)
set(CMAKE_CXX_STANDARD 26)
set(raylib_DIR "C:/Users/7906200/.vcpkg-clion/vcpkg/installed/x64-mingw-dynamic/share/raylib")
set(raylib_INCLUDE_DIR "C:/Users/7906200/.vcpkg-clion/vcpkg/installed/x64-mingw-dynamic/include")
set(nlohmann_json_DIR "C:/Users/7906200/.vcpkg-clion/vcpkg/installed/x64-mingw-dynamic/share/nlohmann_json")
set(unofficial-enet_DIR "C:/Users/7906200/.vcpkg-clion/vcpkg/installed/x64-mingw-dynamic/share/unofficial-enet")
set(raylib_LIBRARY "C:/Users/7906200/.vcpkg-clion/vcpkg/installed/x64-mingw-dynamic/lib/libraylib.dll.a")
find_package(raylib CONFIG REQUIRED)
find_package(unofficial-enet CONFIG REQUIRED)
add_executable(ALLS main.cpp
        network/Server.cpp
        network/Server.h
        network/Client.cpp
        network/Client.h
        network/Utils.cpp)
target_link_libraries(ALLS PRIVATE raylib)
target_link_libraries(ALLS PRIVATE unofficial::enet::enet winmm ws2_32)