cmake_minimum_required(VERSION 4.0)
project(BouncingPlus)
set(CMAKE_CXX_STANDARD 20)
set(raylib_DIR "C:/Users/7906200/.vcpkg-clion/vcpkg/installed/x64-mingw-dynamic/share/raylib")
set(raylib_INCLUDE_DIR "C:/Users/7906200/.vcpkg-clion/vcpkg/installed/x64-mingw-dynamic/include")
set(raylib_LIBRARY "C:/Users/7906200/.vcpkg-clion/vcpkg/installed/x64-mingw-dynamic/lib/libraylib.dll.a")
find_package(raylib CONFIG REQUIRED)
add_executable(BouncingPlus main.cpp
        Game.cpp
        Game.h
        Entity.cpp
        Entity.h
        Player.cpp
        Player.h
        TileManager.cpp
        TileManager.h
        Enemy.cpp
        Enemy.h
        Bullet.cpp
        Bullet.h
        Weapons.h
        Weapons.cpp
        UI.cpp
        UI.h
)
target_link_libraries(BouncingPlus PRIVATE raylib)