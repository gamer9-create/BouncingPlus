cmake_minimum_required(VERSION 4.0)
project(BouncingPlus)
set(CMAKE_CXX_STANDARD 20)
#set(raylib_DIR "C:/Users/7906200/.vcpkg-clion/vcpkg/installed/x64-mingw-dynamic/share/raylib")
#set(raylib_INCLUDE_DIR "C:/Users/7906200/.vcpkg-clion/vcpkg/installed/x64-mingw-dynamic/include")
#set(nlohmann_json_DIR "C:/Users/7906200/.vcpkg-clion/vcpkg/installed/x64-mingw-dynamic/share/nlohmann_json")
#set(raylib_LIBRARY "C:/Users/7906200/.vcpkg-clion/vcpkg/installed/x64-mingw-dynamic/lib/libraylib.dll.a")
find_package(raylib CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
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
        UIManager.cpp
        UIManager.h
        Menu.cpp
        Menu.h
        ParticleManager.cpp
        ParticleManager.h
        LevelLoader.cpp
        LevelLoader.h
        CameraManager.cpp
        CameraManager.h
        EntityManager.cpp
        EntityManager.h
        EntityType.h
        Spawner.cpp
        Spawner.h
        TileType.h
        SoundManager.cpp
        SoundManager.h
)
target_link_libraries(BouncingPlus PRIVATE raylib)
target_link_libraries(BouncingPlus PRIVATE nlohmann_json)