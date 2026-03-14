#include "raylib.h"
#include <iostream>
#include "game/Game.h"
#include "game/ui/Menu.h"
#include "game/saves/Settings.h"
#include "level/LevelLoader.h"

int main(int argc, char *argv[]) {
    InitWindow(1480, 920, "BouncingPlus");
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI | FLAG_VSYNC_HINT);

    SetWindowIcon(LoadImage("assets/img/player.png"));
    InitAudioDevice();

    LevelLoader level_loader = LevelLoader();
    std::map<std::string,json> level_data = level_loader.GetLevelsData();

    Settings gameSettings{};
    gameSettings.LevelData = level_data;
    gameSettings.FrameRate = GetMonitorRefreshRate(0);
    gameSettings.UIAssets = UIAssets();
    gameSettings.UIAssets.Load();
    float LastFramerate = 0;

    Game MainGame = Game(gameSettings);
    Menu MainMenu = Menu(gameSettings);

    bool InGame = false;

    // tip of advice: dont look into any other code file that isnt a manager... youre gonna find some... uhhh... extremely readable code!

    while (!WindowShouldClose()) {
        BeginDrawing();

        if (gameSettings.FrameRate != LastFramerate)
            SetTargetFPS((int)gameSettings.FrameRate);
        LastFramerate=gameSettings.FrameRate;

        if (gameSettings.CursorWindowLock && !IsCursorOnScreen())
        {
            SetMousePosition(min(max(GetMouseX(), 25), GetScreenWidth() - 25), min(max(GetMouseY(), 25), GetScreenHeight() - 25));
        }

        SetMasterVolume(gameSettings.Volume/100.0f);

        if (IsKeyPressed(KEY_F11))
            ToggleFullscreen();

        ClearBackground(BLANK);
        if (InGame) {

            if (MainGame.ShouldReturn)
            {
                InGame = false;
                MainMenu.Reset();
                MainGame.ShouldReturn = false;
                MainGame.Clear();
                ShowCursor();
            } else
            {
                MainGame.Update();
            }

            // i am scared!!! i scare you!!!
        } else {
            MainMenu.Update();
            std::string map = MainMenu.LeaveMenu();
            if (!map.empty()) {
                HideCursor();
                InGame = true;
                MainGame.ShouldReturn = false;
                MainGame.Reload(map);
            }
        }
        DrawFPS(0,0);

        EndDrawing();
    }

    MainMenu.Quit();
    MainGame.Quit();
    gameSettings.UIAssets.UnLoad();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
