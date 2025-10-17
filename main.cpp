#include <iostream>
#include <raylib.h>
#include "Game.h"
#include "Menu.h"

int main() {
    InitWindow(1280, 720, "BouncingPlus");
    SetTargetFPS(240);
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
    SetWindowIcon(LoadImage("assets/img/player.png"));

    Color BackgroundColor = {100, 100, 100, 255};

    Game MainGame = Game();
    Menu MainMenu = Menu();

    MainGame.Reload("assets/maps/untitled.csv");

    bool InGame = false;

    float Background_Dist = 3;
    float grid_space = 36.0f;


    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(BackgroundColor);

        float ParallaxCamX = MainGame.CameraPosition.x / Background_Dist;
        float ParallaxCamY = MainGame.CameraPosition.y / Background_Dist;

        for (int i = -1; i < round(GetScreenHeight() / grid_space)+1; i++) {
            int y = (int)(ParallaxCamY / grid_space);
            DrawLineEx({0, ((y+i)*grid_space) - ParallaxCamY}, {(float) GetScreenWidth(), ((y+i)*grid_space) - ParallaxCamY}, 7, ColorBrightness(WHITE, -0.5f));
        }

        for (int i = -1; i < round(GetScreenWidth() / grid_space)+1; i++) {
            int x = (int)(ParallaxCamX / grid_space);
            DrawLineEx({((x+i)*grid_space) - ParallaxCamX, 0}, {((x+i)*grid_space) - ParallaxCamX, (float) GetScreenHeight()}, 7, ColorBrightness(WHITE, -0.5f));
            DrawLineEx({((x+i)*grid_space) - ParallaxCamX, 0}, {((x+i)*grid_space) - ParallaxCamX, (float) GetScreenHeight()}, 3, WHITE);
        }

        for (int i = -1; i < round(GetScreenHeight() / grid_space)+1; i++) {
            int y = (int)(ParallaxCamY / grid_space);
            DrawLineEx({0, ((y+i)*grid_space) - ParallaxCamY}, {(float) GetScreenWidth(), ((y+i)*grid_space) - ParallaxCamY}, 3, WHITE);
        }

        if (InGame) {
            MainGame.CameraTarget = {MainGame.MainPlayer->BoundingBox.x +
            MainGame.MainPlayer->BoundingBox.width / 2, MainGame.MainPlayer->BoundingBox.y +
            MainGame.MainPlayer->BoundingBox.height / 2};

            MainGame.Update();

            if (IsKeyPressed(KEY_E))
                MainGame.Reload("assets/maps/cpp_level.csv");
        } else {
            MainMenu.Update();
            if (MainMenu.LeaveMenu())
                InGame = true;
        }

        DrawFPS(0,0);
        EndDrawing();
    }

    MainGame.Quit();
    CloseWindow();
    return 0;
}
