cmake_minimum_required(VERSION 4.0)
project(BouncingPlus)
set(CMAKE_CXX_STANDARD 20)
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
        Menu.cpp
        Menu.h
)
target_link_libraries(BouncingPlus PRIVATE raylib)