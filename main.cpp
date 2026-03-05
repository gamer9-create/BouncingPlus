#include "raylib.h"
#include <iostream>
#include "game/Game.h"
#include "game/Menu.h"
#include "level/LevelLoader.h"

int main(int argc, char *argv[]) {
    InitWindow(1480, 920, "BouncingPlus");
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI | FLAG_VSYNC_HINT);

    SetWindowIcon(LoadImage("assets/img/player.png"));
    InitAudioDevice();

    LevelLoader level_loader = LevelLoader();
    std::map<std::string,json> level_data = level_loader.GetLevelsData();

    float MasterVolume = 100.0f;
    bool LockCursor = false;
    float Framerate = 60;
    float LastFramerate = 0;

    Game MainGame = Game(level_data);
    Menu MainMenu = Menu(level_data, &MasterVolume, &Framerate);

    bool InGame = false;

    Music music = LoadMusicStream(string("assets/spookypiano.mp3").c_str());
    SetMusicVolume(music, 1);
    time_t t;
    time(&t);
    tm datetime = *localtime(&t);
    if (to_string(datetime.tm_mon) == "9" && to_string(datetime.tm_mday) == "31" && GetRandomValue(1, 10) == 5) {
        PlayMusicStream(music);
        cout << "spooky scary skel" << endl;
    }
    // tip of advice: dont look into any other code file that isnt a manager... youre gonna find some... uhhh... extremely readable code!

    while (!WindowShouldClose()) {
        BeginDrawing();

        if (Framerate != LastFramerate)
            SetTargetFPS((int)Framerate);
        LastFramerate=Framerate;

        if (LockCursor && !IsCursorOnScreen())
        {
            SetMousePosition(min(max(GetMouseX(), 25), GetScreenWidth() - 25), min(max(GetMouseY(), 25), GetScreenHeight() - 25));
        }

        SetMasterVolume(MasterVolume/100.0f);

        if (IsKeyPressed(KEY_F11))
            ToggleFullscreen();

        ClearBackground(BLANK);
        UpdateMusicStream(music);
        if (InGame) {

            if (MainGame.ShouldReturn)
            {
                InGame = false;
                MainMenu.Reset();
                MainGame.ShouldReturn = false;
            } else
            {
                MainGame.Update();
            }

            // i am scared!!! i scare you!!!
        } else {
            MainMenu.Update();
            std::string map = MainMenu.LeaveMenu();
            if (!map.empty()) {
                InGame = true;
                MainGame.ShouldReturn = false;
                MainGame.Reload(map);
            }
        }
        DrawFPS(0,0);

        EndMode2D();
        EndDrawing();
    }

    MainMenu.Quit();
    MainGame.Quit();
    if (IsMusicStreamPlaying(music))
        StopMusicStream(music);
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
