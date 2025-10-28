#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include "Game.h"
#include "Menu.h"
#include "LevelLoader.h"

int main() {
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "BouncingPlus");
    SetTargetFPS(GetMonitorRefreshRate(0));
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI | FLAG_VSYNC_HINT);

    SetWindowIcon(LoadImage("assets/img/player.png"));
    InitAudioDevice();

    LevelLoader level_loader = LevelLoader();
    std::unordered_map<std::string,json> level_data = level_loader.GetLevelsData();

    Game MainGame = Game(level_data);
    Menu MainMenu = Menu(level_data);

    bool InGame = false;

    bool OnWeb = false;
    #ifdef PLATFORM_WEB
        OnWeb = true;
    #endif

    bool HasAgreed = true;

    if (OnWeb)
        HasAgreed = false;

    Camera2D zoom_cam = {{0, 0}, {0, 0}, 0, 1.0f};

    Music music = LoadMusicStream(("assets/music/game_music" + to_string(GetRandomValue(1, 2)) + ".wav").c_str());
    SetMusicVolume(music, 0.25f);
    //PlayMusicStream(music);

    while (!WindowShouldClose()) {
        BeginDrawing();

        if (IsKeyPressed(KEY_F11) && !OnWeb)
            ToggleFullscreen();

        BeginMode2D(zoom_cam);
        UpdateMusicStream(music);

        ClearBackground(BLANK);

        if (HasAgreed) {

            if (InGame) {

                MainGame.Update(zoom_cam);
                zoom_cam.zoom = lerp(zoom_cam.zoom, MainGame.MainCameraManager.CameraZoom, 4 * GetFrameTime());
                zoom_cam.offset = {((float)GetScreenWidth()/2.0f) * (1.0f-zoom_cam.zoom), ((float)GetScreenHeight()/2.0f) * (1.0f-zoom_cam.zoom)};

                // i am scared!!! i scare you!!!
                if (MainGame.DebugDraw)
                    SetMusicVolume(music, 0);
                else
                    SetMusicVolume(music, 0.25f);
            } else {
                zoom_cam = {{0, 0}, {0, 0}, 0, 1.0f};
                MainMenu.Update();
                std::string map = MainMenu.LeaveMenu();
                if (!map.empty()) {
                    InGame = true;
                    MainGame.Reload(map);
                }
            }
            DrawFPS(0,0);
        } else {
            DrawText("WARNING: This game runs better on Native Hardware! The web version may be glitchy/laggy. Please L to skip this warning.", 20, 20, 20, RED);
            DrawText("Make sure to fullscreen before playing too!", 20, 40, 20, RED);
            if (IsKeyPressed(KEY_L))
                HasAgreed = true;
        }

        EndMode2D();
        EndDrawing();
    }

    MainMenu.Quit();
    MainGame.Quit();
    StopMusicStream(music);
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
