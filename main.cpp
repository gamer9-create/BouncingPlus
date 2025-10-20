#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include "Game.h"
#include "Menu.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;


int main() {
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "BouncingPlus");
    SetTargetFPS(240);
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);

    SetWindowIcon(LoadImage("assets/img/player.png"));
    InitAudioDevice();

    Color BackgroundColor = {100, 100, 100, 255};

    Game MainGame = Game();
    Menu MainMenu = Menu();

    bool InGame = false;

    float Background_Dist = 3;
    float grid_space = 36.0f;

    bool OnWeb = false;
    #ifdef PLATFORM_WEB
        OnWeb = true;
    #endif

    bool HasAgreed = true;

    if (OnWeb)
        HasAgreed = false;

    Camera2D zoom_cam = {{0, 0}, {0, 0}, 0, 1.0f};
    while (!WindowShouldClose()) {
        BeginDrawing();

        BeginMode2D(zoom_cam);

        if (IsKeyPressed(KEY_F11) && !OnWeb)
            ToggleFullscreen();

        ClearBackground(BackgroundColor);

        if (HasAgreed) {

            if (InGame) {

                float ParallaxCamX = MainGame.CameraPosition.x / Background_Dist;
                float ParallaxCamY = MainGame.CameraPosition.y / Background_Dist;

                for (int i = -1; i < round(GetScreenHeight() / grid_space)+1; i++) {
                    int y = (int)(ParallaxCamY / grid_space);
                    DrawLineEx({0, ((y+i)*grid_space) - ParallaxCamY}, {(float) GetScreenWidth(), ((y+i)*grid_space) - ParallaxCamY}, 7, ColorBrightness(WHITE, -0.5f));
                }

                for (int i = -1; i < round(GetScreenWidth() / grid_space)+1; i++) {
                    int x = (int)(ParallaxCamX / grid_space);
                    DrawLineEx({((x+i)*grid_space) - ParallaxCamX, 0}, {((x+i)*grid_space) - ParallaxCamX, (float) GetScreenHeight()}, 7, ColorBrightness(WHITE, -0.5f));
                    DrawLineEx({((x+i)*grid_space) - ParallaxCamX, 0}, {((x+i)*grid_space) - ParallaxCamX, (float) GetScreenHeight()}, 3, ColorAlpha(WHITE, 0.5f));
                }

                for (int i = -1; i < round(GetScreenHeight() / grid_space)+1; i++) {
                    int y = (int)(ParallaxCamY / grid_space);
                    DrawLineEx({0, ((y+i)*grid_space) - ParallaxCamY}, {(float) GetScreenWidth(), ((y+i)*grid_space) - ParallaxCamY}, 3, ColorAlpha(WHITE, 0.5f));
                }

                MainGame.CameraTarget = {MainGame.MainPlayer->BoundingBox.x +
                MainGame.MainPlayer->BoundingBox.width / 2, MainGame.MainPlayer->BoundingBox.y +
                MainGame.MainPlayer->BoundingBox.height / 2};

                MainGame.Update();

                zoom_cam.zoom = lerp(zoom_cam.zoom, MainGame.CameraZoom, 4 * GetFrameTime());
                zoom_cam.offset = {((float)GetScreenWidth()/2.0f) * (1.0f-zoom_cam.zoom), ((float)GetScreenHeight()/2.0f) * (1.0f-zoom_cam.zoom)};

                // i am scared!!! i scare you!!!

                if (IsKeyPressed(KEY_X))
                    MainGame.DebugDraw = !MainGame.DebugDraw;
                if (MainGame.DebugDraw) {

                }
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
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
