//
// Created by lalit on 10/16/2025.
//

#include "Menu.h"
#include <nlohmann/json.hpp>
#include "UIUtils.h"
#include <raymath.h>

using namespace std;

Menu::Menu(Settings& GameSettings)
{
    this->GameSettings = &GameSettings;
    Reset();
    PlaySound(this->GameSettings->UIAssets.MikuMusic);
}

void Menu::Reset()
{
    Map = "";
    TargetMap = "";
    TitleImgY = -GameSettings->UIAssets.TitleImg.height;
    TitleImgOffsetY = 0;
    PlayButtonOffsetY = -100;
    SettingsButtonOffsetY = -200;
    CreditsButtonOffsetY = -300;
    CameraX = 0;
    CameraTargetX = 0;
    Offset1 = 0;
    Offset2 = 0;
    Offset3 = 0;
    isStarting = true;
    BlackTransparency= 1.0f;
    MikuOffset = 0.0f;
    MenuImgOffsetY = GetRandomValue(0, GameSettings->UIAssets.MenuImg.height);
    MovingToGame = false;
    MousePos = {0,0};
}

void Menu::LevelSelect()
{
    Rectangle play_bbox = {(GetScreenWidth()/2.0f) - GetScreenWidth() - (int)(GameSettings->UIAssets.ButtonImg.width/2.0f), GetScreenHeight() - 106 + Offset1,150,56};
    if (Button({play_bbox.x - CameraX, play_bbox.y, play_bbox.width, play_bbox.height}, GetMousePosition(), GameSettings->UIAssets.ButtonImg, GameSettings->UIAssets.ButtonClick, "BACK")) {
        CameraTargetX=0;
    }
    DrawText("LEVEL SELECT", (int)(GetScreenWidth()/2.0f) - (int)(MeasureText("LEVEL SELECT", 50)/2.0f)-CameraX-GetScreenWidth(), 100+(Offset2/1.5f), 50, WHITE);
    int i = 0;
    for (auto& [name, data] : GameSettings->LevelData)
    {
        i = data["order"];
        Rectangle r = {-710.0f, (float)165 + (140 * i)-Offset3, (float)MeasureText(name.c_str(), 90)+20.0f, 110.0f};
        DrawRectangleRec({r.x-CameraX,r.y,r.width,r.height}, ColorAlpha(BLACK, 0.5f));
        Color c = RED;
        if (TargetMap==name) {
            c = GREEN;
            float comb = (Offset1 + Offset2 + Offset3)/3;
            DrawRectangle(-(GetScreenWidth()-125)-CameraX, 200-comb, GetScreenWidth()-900, GetScreenHeight()-450, ColorAlpha(BLACK, 0.5f));
            DrawText(name.c_str(), -(GetScreenWidth()-145)-CameraX, 220-comb, 50, WHITE);

            std::string description = data["description"];

            if (!description.empty())
            {
                float alloc_size = GetScreenWidth() - 940;
                int lines = ceil(MeasureText(description.c_str(), 25) / alloc_size);

                int str_prog = 0;
                int last_str_prog = 0;

                for (int i = 0; i < lines; i++)
                {
                    last_str_prog = str_prog;
                    while (MeasureText(description.substr(last_str_prog, str_prog - last_str_prog).c_str(), 25) < alloc_size && str_prog < description.length())
                        str_prog++;

                    std::string txt = description.substr(last_str_prog, str_prog - last_str_prog);
                    if (txt.front() == ' ')
                        txt.erase(txt.begin());
                    //description.substr(max((float)(i * (description.size() / lines)), 0.0f), min((i+1) * (description.size() / lines), description.size())).c_str()

                    DrawText(txt.c_str(), -(GetScreenWidth()-145)-CameraX, 270-comb + (35 * i), 25, WHITE);
                }
            }

            DrawText(("Difficulty level: "+to_string(data["difficulty"])).c_str(), -(GetScreenWidth()-145)-CameraX, 200-comb + GetScreenHeight()-450 - 70 - 64, 25, ColorBrightness(RED, -1 + ((float)data["difficulty"] * 0.25f) ));
            Rectangle play_bbox = {(float)-(GetScreenWidth()-145), 200-comb + GetScreenHeight()-450 - 70,150,56};
            if (Button({play_bbox.x - CameraX, play_bbox.y, play_bbox.width, play_bbox.height}, GetMousePosition(), GameSettings->UIAssets.ButtonImg, GameSettings->UIAssets.ButtonClick, "PLAY")) {
                MovingToGame = true;
            }
        }
        DrawText(name.c_str(), -700 - CameraX, 175 + (140 * i)-Offset3, 90, c);
        if (CheckCollisionPointRec(MousePos, r)) {
            DrawRectangleLinesEx({r.x-CameraX,r.y,r.width,r.height}, 4, WHITE);
            TargetMap = IsMouseButtonPressed(0) ? (TargetMap != name ? name : "") : TargetMap;
        }
    }
}

void Menu::Update() {
    if (!MovingToGame) {
        CameraX = Lerp(CameraX, CameraTargetX, 2 * GetFrameTime());
        MousePos = {(float)GetMouseX() + CameraX, (float)GetMouseY()};
    }

    if (IsCursorHidden())
        ShowCursor();

    Offset1 = -sin((GetTime()-20.0f) * 3.5f) * 15;
    Offset2 = -sin((GetTime()+20.0f) * 3.5f) * 15;
    Offset3 = -sin(GetTime() * 3.5f) * 15;

    TitleImgY = Lerp(TitleImgY, -100, 5 * GetFrameTime());
    TitleImgOffsetY = Offset1;
    if (abs(TitleImgY + 100) <= 10)
        PlayButtonOffsetY = Lerp(PlayButtonOffsetY, 550, 5 * GetFrameTime());
    if (abs(PlayButtonOffsetY - 550) <= 45)
        SettingsButtonOffsetY = Lerp(SettingsButtonOffsetY, 624, 7 * GetFrameTime());
    if (abs(SettingsButtonOffsetY - 624) <= 45)
        CreditsButtonOffsetY = Lerp(CreditsButtonOffsetY, 698, 9 * GetFrameTime());

    MenuImgOffsetY += GetFrameTime()*15;
    float zoom = 1.35f;
    DrawTexturePro(GameSettings->UIAssets.MenuImg, {0, MenuImgOffsetY, (float)GetScreenWidth(), (float)GetScreenHeight()},
        {GetScreenWidth()/2 - (GetScreenWidth() * zoom)/2 - CameraX/10, GetScreenHeight()/2 - (GetScreenHeight() * zoom)/2, (float)GetScreenWidth() * zoom, (float)GetScreenHeight() * zoom},
        {0,0},
        0,
        WHITE
        );

    DrawTexture(GameSettings->UIAssets.TitleImg, (int)(GetScreenWidth()/2.0f) - (int)(GameSettings->UIAssets.TitleImg.width/2.0f)-CameraX, (int)TitleImgY - (int)TitleImgOffsetY, WHITE);

    GameSettings->DisplaySettings(Vector2{-CameraX, 0}, Offset1, Offset2);

    Rectangle play_bbox = {(GetScreenWidth()/2.0f) - (int)(GameSettings->UIAssets.ButtonImg.width/2.0f), (float)PlayButtonOffsetY +Offset3,150,56};
    if (Button({play_bbox.x - CameraX, play_bbox.y, play_bbox.width, play_bbox.height}, GetMousePosition(), GameSettings->UIAssets.ButtonImg, GameSettings->UIAssets.ButtonClick, "PLAY")) {
        CameraTargetX=-GetScreenWidth();
    }
    DrawTexture(GameSettings->UIAssets.CreditsImg,(GetScreenWidth()*2)-CameraX, 0, WHITE);

    if (Button({GetScreenWidth()*2 + (GetScreenWidth() / 2.0f) - 75.0f - CameraX, GetScreenHeight() - 106.0f, 150, 56}, GetMousePosition(),
        GameSettings->UIAssets.ButtonImg, GameSettings->UIAssets.ButtonClick, "BACK") ||
        Button({GetScreenWidth() + (GetScreenWidth() / 2.0f) - 75.0f - CameraX, GetScreenHeight() - 106.0f + (Offset2 + Offset3)/2, 150, 56}, GetMousePosition(),
            GameSettings->UIAssets.ButtonImg, GameSettings->UIAssets.ButtonClick, "BACK"))
        CameraTargetX=0;

    if (Button({(GetScreenWidth()/2.0f) - (int)(GameSettings->UIAssets.ButtonImg.width/2.0f) - CameraX, (float)SettingsButtonOffsetY + (Offset2 + Offset3)/2,150,56}
        ,GetMousePosition(), GameSettings->UIAssets.ButtonImg, GameSettings->UIAssets.ButtonClick, "SETTINGS"))
        CameraTargetX=GetScreenWidth();

    if (Button({(GetScreenWidth()/2.0f) - (int)(GameSettings->UIAssets.ButtonImg.width/2.0f) - CameraX, (float)CreditsButtonOffsetY +Offset2,150,56},
        GetMousePosition(), GameSettings->UIAssets.ButtonImg, GameSettings->UIAssets.ButtonClick, "CREDITS"))
        CameraTargetX=GetScreenWidth()*2;

    if (isStarting && BlackTransparency > 0)
        BlackTransparency -= 0.65f * GetFrameTime();
    else
    {
        isStarting = false;
    }
    if (MovingToGame)
        BlackTransparency += 0.65f * GetFrameTime();
    if (MovingToGame && BlackTransparency >= 1.0f) {
        Map = TargetMap;
        StopSound(GameSettings->UIAssets.MikuMusic);
    }

    DrawTexture(GameSettings->UIAssets.MikuImg, -75, GetScreenHeight() - 20 + MikuOffset, WHITE);
    if (GetMouseX() < 250 && GetMouseY() > GetScreenHeight() - 70) {
        MikuOffset = lerp(MikuOffset, -500, 10*GetFrameTime());
        if (!IsSoundPlaying(GameSettings->UIAssets.MikuMusic)) {
            SetSoundVolume(GameSettings->UIAssets.MikuMusic, 0.2f);
            ResumeSound(GameSettings->UIAssets.MikuMusic);
        }
    } else {
        MikuOffset = lerp(MikuOffset, 0, 10*GetFrameTime());
        PauseSound(GameSettings->UIAssets.MikuMusic);
    }

    LevelSelect();
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), ColorAlpha(BLACK, BlackTransparency));
}

std::string Menu::LeaveMenu() {
    return Map;
}

void Menu::Quit() {
    Reset();
}