cmake_minimum_required(VERSION 4.0)
project(BouncingPlus)
set(CMAKE_CXX_STANDARD 20)
find_package(raylib CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(unofficial-enet CONFIG REQUIRED)
add_executable(BouncingPlus main.cpp
        game/Game.cpp
        game/Game.h
        entities/Entity.cpp
        entities/Entity.h
        entities/subentities/Player.cpp
        entities/subentities/Player.h
        level/TileManager.cpp
        level/TileManager.h
        entities/subentities/Enemy.cpp
        entities/subentities/Enemy.h
        entities/subentities/Bullet.cpp
        entities/subentities/Bullet.h
        entities/Weapons.h
        entities/Weapons.cpp
        game/managers/UIManager.cpp
        game/managers/UIManager.h
        game/Menu.cpp
        game/Menu.h
        game/managers/ParticleManager.cpp
        game/managers/ParticleManager.h
        level/LevelLoader.cpp
        level/LevelLoader.h
        game/managers/CameraManager.cpp
        game/managers/CameraManager.h
        entities/EntityManager.cpp
        entities/EntityManager.h
        entities/EntityType.h
        entities/subentities/Spawner.cpp
        entities/subentities/Spawner.h
        level/TileType.h
        game/managers/SoundManager.cpp
        game/managers/SoundManager.h
        entities/subentities/UpgradeStation.cpp
        entities/subentities/UpgradeStation.h
        entities/bosses/FaceBoss.cpp
        entities/bosses/FaceBoss.h
        entities/Powerups.cpp
        entities/Powerups.h
        game/managers/GameModeManager.cpp
        game/managers/GameModeManager.h
        game/saves/Achievements.cpp
        game/saves/Achievements.h
        game/saves/Saves.cpp
        game/saves/Saves.h
        entities/subentities/behaviors/EnemyBehavior.cpp
        entities/subentities/behaviors/EnemyBehavior.h
        entities/subentities/behaviors/WeaponBehavior.cpp
        entities/subentities/behaviors/WeaponBehavior.h
)
target_link_libraries(BouncingPlus PRIVATE raylib)
target_link_libraries(BouncingPlus PRIVATE nlohmann_json)
target_link_libraries(BouncingPlus PRIVATE unofficial::enet::enet winmm ws2_32)