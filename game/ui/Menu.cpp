//
// Created by lalit on 10/16/2025.
//

#include "Menu.h"

#include <iostream>
#include <nlohmann/json.hpp>
#include "UIUtils.h"
#include <raymath.h>

using namespace std;

float Menu::MusicLevel = 0.0f;
void Menu::AudioCallback(void* buffer, unsigned int frames)
{
    float *samples = (float *)buffer;

    float sum = 0.0f;
    for (unsigned int i = 0; i < frames * 2; i++) {
        sum += samples[i] * samples[i];
    }

    MusicLevel = sqrtf(sum / (frames * 2));
}

Menu::Menu(Settings& GameSettings)
{
    MusicLevel = 0.0f;
    this->GameSettings = &GameSettings;
    Reset();
    PlaySound(this->GameSettings->UIAssets.MikuMusic);

    AttachAudioStreamProcessor(this->GameSettings->UIAssets.MainMenuMusic.stream, this->AudioCallback);
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
    MusicLevel = 0.0f;
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

void Menu::Credits()
{
    Vector2 CreditsPanelSize = Vector2{(float)GetScreenWidth() - 200.0f, (float)GetScreenHeight() - 180.0f};
    Rectangle CreditsPanelRectangle = {(float)GetScreenWidth() * 2.0f + (float)GetScreenWidth()/2.0f - CreditsPanelSize.x / 2.0f, 25 - Offset1, CreditsPanelSize.x, CreditsPanelSize.y};

    DrawRectangleRec({CreditsPanelRectangle.x - CameraX, CreditsPanelRectangle.y, CreditsPanelRectangle.width, CreditsPanelRectangle.height}, ColorAlpha(BLACK, 0.5f));

    float CreditsTextWidth = MeasureText("CREDITS", 55.0f);
    DrawText("CREDITS", CreditsPanelRectangle.x + CreditsPanelRectangle.width/2.0f - CreditsTextWidth/2.0f - CameraX, CreditsPanelRectangle.y + 25.0f, 55.0f, WHITE);

    DrawLineEx(Vector2{CreditsPanelRectangle.x + 25.0f - CameraX, CreditsPanelRectangle.y + 100}, Vector2{CreditsPanelRectangle.x + CreditsPanelSize.x - 25.0f - CameraX, CreditsPanelRectangle.y + 100}, 4, WHITE);

    DrawTexture(GameSettings->UIAssets.RolponPFPImg, CreditsPanelRectangle.x + 25 - CameraX, CreditsPanelRectangle.y + 125.0f, WHITE);
    DrawTexture(GameSettings->UIAssets.CozPFPImg, CreditsPanelRectangle.x + 25 - CameraX, CreditsPanelRectangle.y + 406.0f, WHITE);

    DrawTexture(GameSettings->UIAssets.InkyPFPImg, CreditsPanelRectangle.x + 25 + (CreditsPanelSize.x-50)/2 - CameraX, CreditsPanelRectangle.y + 125.0f, WHITE);
    DrawTexture(GameSettings->UIAssets.JayPFPImg, CreditsPanelRectangle.x + 25 + (CreditsPanelSize.x-50)/2 - CameraX, CreditsPanelRectangle.y + 406.0f, WHITE);

    float R = 127.0f + 127.0f * sin(GetTime());
    float G = 127.0f + 127.0f * cos(GetTime());
    float B = 127.0f + 127.0f * sin(GetTime() + 10);

    DrawText("Rolpon", CreditsPanelRectangle.x + 291 - CameraX, CreditsPanelRectangle.y + 125.0f, 45.0f, Color{(unsigned char) R, (unsigned char) G, (unsigned char) B, 255});

    DrawText("Coz", CreditsPanelRectangle.x + 291 - CameraX, CreditsPanelRectangle.y + 406.0f, 45.0f, PURPLE);

    DrawText("inkyrblx", CreditsPanelRectangle.x + 291 + (CreditsPanelSize.x-50)/2 - CameraX, CreditsPanelRectangle.y + 125.0f, 45.0f, BROWN);

    DrawText("jaymbermations", CreditsPanelRectangle.x + 291 + (CreditsPanelSize.x-50)/2 - CameraX, CreditsPanelRectangle.y + 406.0f, 45.0f, ORANGE);

    DrawText("Programmer & Game Director", CreditsPanelRectangle.x + 291 - CameraX, CreditsPanelRectangle.y + 170.0f, 20, WHITE);

    DrawText("Playtester & Game Director", CreditsPanelRectangle.x + 291 - CameraX, CreditsPanelRectangle.y + 451.0f, 20, WHITE);

    DrawText("Playtester & SFX/Music", CreditsPanelRectangle.x + 291 + (CreditsPanelSize.x-50)/2 - CameraX, CreditsPanelRectangle.y + 170.0f, 20, WHITE);

    DrawText("Playtester & Linux Port", CreditsPanelRectangle.x + 291 + (CreditsPanelSize.x-50)/2 - CameraX, CreditsPanelRectangle.y + 406.0f + 45, 20, WHITE);

    float ThankYouTextWidth = MeasureText("Finally, thank YOU, for playing!", 40);
    DrawText("Finally, thank YOU, for playing!", CreditsPanelRectangle.x + CreditsPanelSize.x/2 - ThankYouTextWidth/2 - CameraX, CreditsPanelRectangle.y + CreditsPanelSize.y - 50, 40, WHITE);

    float FntSize = 24;
    float TxSize = MeasureText("Support me on YouTube!", FntSize);
    Rectangle r = {0, 0, TxSize + FntSize + 10, FntSize + 10};
    r.x = GetScreenWidth() - r.width;
    r.y = GetScreenHeight() - r.height;

    float TransparencyDiv = 2.0f;

    if (CheckCollisionPointRec(GetMousePosition(), r))
    {
        TransparencyDiv = 1.0f;
        if (IsMouseButtonPressed(0))
            OpenURL("https://www.youtube.com/@rolpon2871");
    }

    DrawRectangleRec(r, ColorAlpha(BLACK, 0.5f / TransparencyDiv));

    DrawText("Support me on YouTube!", r.x + 5, r.y + 5, FntSize, ColorAlpha(WHITE, 1.0f / TransparencyDiv));
    DrawTextureEx(GameSettings->UIAssets.YTImg, Vector2{r.x + 12 + TxSize, r.y + 5}, 0, FntSize / 512.0f, ColorAlpha(WHITE, 1.0f / TransparencyDiv));
}

void Menu::Update() {
    if (!MovingToGame) {
        CameraX = Lerp(CameraX, CameraTargetX, 2 * GetFrameTime());
        MousePos = {(float)GetMouseX() + CameraX, (float)GetMouseY()};
    }

    if (!IsMusicStreamPlaying(GameSettings->UIAssets.MainMenuMusic))
        PlayMusicStream(GameSettings->UIAssets.MainMenuMusic);
    UpdateMusicStream(GameSettings->UIAssets.MainMenuMusic);

    if (IsCursorHidden())
        ShowCursor();

    MenuMusicLevel = Lerp(MenuMusicLevel, MusicLevel, 8.5f * GetFrameTime());

    Offset1 = -sin((GetTime()-20.0f) * 3.5f) * 15;
    Offset2 = -sin((GetTime()+20.0f) * 3.5f) * 15;
    Offset3 = -sin(GetTime() * 3.5f) * 15;

    Offset1 += MenuMusicLevel * 80.5f;
    Offset2 += MenuMusicLevel * 60.5f;
    Offset3 += MenuMusicLevel * 40.5f;

    TitleImgY = Lerp(TitleImgY, -100, 5 * GetFrameTime());
    TitleImgOffsetY = Offset1;
    if (abs(TitleImgY + 100) <= 10)
        PlayButtonOffsetY = Lerp(PlayButtonOffsetY, 550, 5 * GetFrameTime());
    if (abs(PlayButtonOffsetY - 550) <= 45)
        SettingsButtonOffsetY = Lerp(SettingsButtonOffsetY, 624, 7 * GetFrameTime());
    if (abs(SettingsButtonOffsetY - 624) <= 45)
        CreditsButtonOffsetY = Lerp(CreditsButtonOffsetY, 698, 9 * GetFrameTime());

    MenuImgOffsetY += GetFrameTime()*15;
    float zoom = 1.45f;
    DrawTexturePro(GameSettings->UIAssets.MenuImg, {0, MenuImgOffsetY, 1184.0f, 736.0f},
        {(float)GetScreenWidth() / 2.0f - CameraX/10, (float)GetScreenHeight() / 2.0f, (float)GetScreenWidth() * zoom, (float)GetScreenHeight() * zoom},
        {(float)GetScreenWidth() * zoom / 2.0f, (float)GetScreenHeight() * zoom / 2.0f},
        0,
        WHITE
        );

    DrawTexture(GameSettings->UIAssets.TitleImg, (int)(GetScreenWidth()/2.0f) - (int)(GameSettings->UIAssets.TitleImg.width/2.0f)-CameraX, (int)TitleImgY - (int)TitleImgOffsetY, WHITE);

    GameSettings->DisplaySettings(Vector2{-CameraX, 0}, Offset1, Offset2);

    Rectangle play_bbox = {(GetScreenWidth()/2.0f) - (int)(GameSettings->UIAssets.ButtonImg.width/2.0f), (float)PlayButtonOffsetY +Offset3,150,56};
    if (Button({play_bbox.x - CameraX, play_bbox.y, play_bbox.width, play_bbox.height}, GetMousePosition(), GameSettings->UIAssets.ButtonImg, GameSettings->UIAssets.ButtonClick, "PLAY")) {
        CameraTargetX=-GetScreenWidth();
    }

    Credits();

    if (Button({GetScreenWidth()*2 + (GetScreenWidth() / 2.0f) - 75.0f - CameraX, GetScreenHeight() - 106.0f + (Offset1 + Offset3)/2, 150, 56}, GetMousePosition(),
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
        StopMusicStream(GameSettings->UIAssets.MainMenuMusic);
    }

    DrawTexture(GameSettings->UIAssets.MikuImg, -75, GetScreenHeight() - 20 + MikuOffset, WHITE);
    if (GetMouseX() < 250 && GetMouseY() > GetScreenHeight() - 70) {
        MikuOffset = lerp(MikuOffset, -500, 10*GetFrameTime());
        if (!IsSoundPlaying(GameSettings->UIAssets.MikuMusic)) {
            SetSoundVolume(GameSettings->UIAssets.MikuMusic, 0.2f);
            ResumeSound(GameSettings->UIAssets.MikuMusic);
            SetMusicVolume(GameSettings->UIAssets.MainMenuMusic, 0);
        }
    } else {
        MikuOffset = lerp(MikuOffset, 0, 10*GetFrameTime());
        PauseSound(GameSettings->UIAssets.MikuMusic);

        SetMusicVolume(GameSettings->UIAssets.MainMenuMusic, 1.0f - BlackTransparency);
    }

    LevelSelect();

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), ColorAlpha(BLACK, BlackTransparency));
}

std::string Menu::LeaveMenu() {
    return Map;
}

void Menu::Quit() {
    Reset();
    DetachAudioStreamProcessor(GameSettings->UIAssets.MainMenuMusic.stream, AudioCallback);
}