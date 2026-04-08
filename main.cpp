#include "raylib.h"
#include <iostream>
#include "game/Game.h"
#include "game/ui/Menu.h"
#include "game/core/SharedManager.h"
#include "level/LevelLoader.h"

int main(int argc, char *argv[]) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);

    InitWindow(1480, 920, "BouncingPlus");
    InitAudioDevice();

    Image t =LoadImage("assets/img/player.png");
    SetWindowIcon(t);
    UnloadImage(t);

    LevelLoader level_loader = LevelLoader();
    std::map<std::string,json> level_data = level_loader.GetLevelsData();

    SharedManager SharedManager{};
    SharedManager.LevelData = level_data;
    SharedManager.FrameRate = max(min(GetMonitorRefreshRate(GetCurrentMonitor()) + 60,240),0);
    SharedManager.UIAssets = UIAssets();
    SharedManager.UIAssets.Load();
    SharedManager.Controls.SetDefaultBindings();

    Game MainGame = Game(SharedManager);
    Menu MainMenu = Menu(SharedManager);

    bool InGame = false;

    SetWindowSize(GetMonitorWidth(GetCurrentMonitor()) / 1.2f, GetMonitorHeight(GetCurrentMonitor()) / 1.2f);
    SetWindowPosition(GetMonitorWidth(GetCurrentMonitor())/2 - GetRenderWidth()/2, GetMonitorHeight(GetCurrentMonitor())/2 - GetRenderHeight()/2);
    SetExitKey(KEY_NULL);

    // tip of advice: dont look into any other code file that isnt a manager... youre gonna find some... uhhh... extremely readable code!

    while (!WindowShouldClose()) {
        BeginDrawing();

        SharedManager.Update();

        ClearBackground(BLANK);
        if (InGame) {
            if (MainGame.ShouldReturn) {
                InGame = false;
                MainMenu.Reset();
                MainGame.ShouldReturn = false;
                MainGame.Clear();
                ShowCursor();
            } else
                MainGame.Update();
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
    SharedManager.Quit();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
