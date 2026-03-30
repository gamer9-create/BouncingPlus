#include "raylib.h"
#include <iostream>
#include "game/Game.h"
#include "game/ui/Menu.h"
#include "game/core/SharedManager.h"
#include "level/LevelLoader.h"

int main(int argc, char *argv[]) {
    InitWindow(1480, 920, "BouncingPlus");

    Image t =LoadImage("assets/img/player.png");
    SetWindowIcon(t);
    InitAudioDevice();

    UnloadImage(t);

    LevelLoader level_loader = LevelLoader();
    std::map<std::string,json> level_data = level_loader.GetLevelsData();

    SharedManager SharedManager{};
    SharedManager.LevelData = level_data;
    SharedManager.FrameRate = GetMonitorRefreshRate(GetCurrentMonitor()) + 60;
    SharedManager.UIAssets = UIAssets();
    SharedManager.UIAssets.Load();
    SharedManager.Controls.SetDefaultBindings();

    Game MainGame = Game(SharedManager);
    Menu MainMenu = Menu(SharedManager);

    bool InGame = false;

    // tip of advice: dont look into any other code file that isnt a manager... youre gonna find some... uhhh... extremely readable code!

    SetWindowSize(GetMonitorWidth(GetCurrentMonitor()) / 1.4f, GetMonitorHeight(GetCurrentMonitor()) / 1.4f);
    SetWindowPosition(GetMonitorWidth(GetCurrentMonitor())/2 - GetScreenWidth()/2, GetMonitorHeight(GetCurrentMonitor())/2 - GetScreenHeight()/2);

    while (!WindowShouldClose()) {
        BeginDrawing();

        SharedManager.Update();

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
    SharedManager.UIAssets.UnLoad();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
