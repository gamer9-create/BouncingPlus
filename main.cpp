#include <iostream>
#include <raylib.h>
#include "Game.h"
#include "Menu.h"

int main() {
    cout << "PLEASE????" << endl;
    InitWindow(1280, 720, "BouncingPlus");
    SetTargetFPS(240);
    SetWindowIcon(LoadImage("assets/img/player.png"));

    Color BackgroundColor = Color(100, 100, 100, 255);

    Game MainGame = Game();
    Menu MainMenu = Menu();

    bool InGame = false;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BackgroundColor);

        if (InGame) {
            MainGame.CameraTarget = Vector2(MainGame.MainPlayer->BoundingBox.x +
            MainGame.MainPlayer->BoundingBox.width / 2, MainGame.MainPlayer->BoundingBox.y +
            MainGame.MainPlayer->BoundingBox.height / 2);

            MainGame.Update();

            if (IsKeyPressed(KEY_E))
                MainGame.Reload("assets/maps/level_4.csv");
        } else {
            MainMenu.Update();
        }

        DrawFPS(0,0);
        EndDrawing();
    }

    MainGame.Quit();
    CloseWindow();
    return 0;
}
