#include "raylib.h"
#include <iostream>
#include <raymath.h>
#include "game/Game.h"
#include "game/Menu.h"
#include "game/network/Server.h"
#include "level/LevelLoader.h"

void singleplayer() {
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

    Music music = LoadMusicStream(string("assets/spookypiano.mp3").c_str());
    SetMusicVolume(music, 1);
    time_t t;
    time(&t);
    struct tm datetime = *localtime(&t);
    //cout << (to_string(datetime.tm_mon) == "9" && to_string(datetime.tm_mday) == "31") << endl;
    if (to_string(datetime.tm_mon) == "9" && to_string(datetime.tm_mday) == "31" && GetRandomValue(1, 10) == 5) {
        PlayMusicStream(music);
        cout << "spooky scary skel" << endl;
    }

    while (!WindowShouldClose()) {
        BeginDrawing();

        if (IsKeyPressed(KEY_F11) && !OnWeb)
            ToggleFullscreen();

        BeginMode2D(zoom_cam);

        ClearBackground(BLANK);
        UpdateMusicStream(music);
        if (HasAgreed) {

            if (InGame) {

                if (MainGame.ShouldReturn)
                {
                    InGame = false;
                    MainMenu.Reset();
                    MainGame.ShouldReturn = false;
                } else
                {
                    MainGame.Update(zoom_cam);
                    zoom_cam.zoom = lerp(zoom_cam.zoom, MainGame.MainCameraManager.CameraZoom, 4 * GetFrameTime());
                    zoom_cam.offset = {((float)GetScreenWidth()/2.0f) * (1.0f-zoom_cam.zoom), ((float)GetScreenHeight()/2.0f) * (1.0f-zoom_cam.zoom)};
                }

                // i am scared!!! i scare you!!!
            } else {
                zoom_cam = {{0, 0}, {0, 0}, 0, 1.0f};
                MainMenu.Update();
                std::string map = MainMenu.LeaveMenu();
                if (!map.empty()) {
                    InGame = true;
                    MainGame.ShouldReturn = false;
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
    if (IsMusicStreamPlaying(music))
        StopMusicStream(music);
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
}

void client() {

}

void server() {
    Server myServer = Server("127.0.0.1", 8080);
    myServer.Start();
    myServer.Stop();
}

int main(int argc, char *argv[]) {
    cout << "instant crash vro\n";

    if (argc == 0 || argv[0] == "singleplayer") {
        singleplayer();
    } else if (argc > 0 && argv[0] == "server") {
        server();
    } else if (argc > 0 && argv[0] == "client") {
        client();
    }

    return 0;
}
