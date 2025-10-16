#include <iostream>
#include <raylib.h>
#include "Game.h"

int main() {
    cout << "PLEASE????" << endl;
    InitWindow(1280, 720, "BouncingPlus");
    SetTargetFPS(240);
    SetWindowIcon(LoadImage("assets/img/player.png"));

    Color BackgroundColor = Color(100, 100, 100, 255);

    Game MainGame = Game();
    MainGame.Reload("assets/maps/level_4.csv");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BackgroundColor);

        MainGame.CameraTarget = Vector2(MainGame.MainPlayer->BoundingBox.x +
        MainGame.MainPlayer->BoundingBox.width / 2, MainGame.MainPlayer->BoundingBox.y +
        MainGame.MainPlayer->BoundingBox.height / 2);

        MainGame.Update();

        if (IsKeyPressed(KEY_E))
            MainGame.Reload("assets/maps/level_4.csv");
        DrawFPS(0,0);
        EndDrawing();
    }

    MainGame.Quit();
    CloseWindow();
    return 0;
}
