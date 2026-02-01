//
// Created by lalit on 10/23/2025.
//

#include "CameraManager.h"

#include <iostream>
#include <raymath.h>

#include "../Game.h"

void CameraManager::Clear() {
    if (CamTextureInitialized)
        UnloadRenderTexture(CameraRenderTexture);
    CameraPosition = Vector2(0.0f, 0.0f);
    CameraTarget = Vector2(0.0f, 0.0f);
    CameraPositionUnaffected = {0, 0};
    CameraSpeed = 20.0f;
    CameraZoom = 1.0f;
    CameraShakes = 0;
    CameraShakeIntensity = 0;
    CameraShakeOffset = {0, 0};
    CameraShakeTimer = GetTime();
    CameraRenderTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    BackgroundDepth = 3;
    BackgroundGridSize = 36;
    BackgroundColor = {100, 100, 100, 255};
    CamTextureInitialized = true;
    BGTexture = GetRandomValue(1, 3);
    ShowLines = GetRandomValue(1, 1000) == 783;
    ZoomResetTimer= 0;
    ShaderDraw= false;
    ShaderPixelPower = 2;
    uWidth = -1;
    uHeight = -1;
    uWidth2 = -1;
    uHeight2 = -1;
    uPixelSize = -1;
    RaylibCamera = {{0,0}, {0, 0}, 0, 1.0f};
}

CameraManager::CameraManager(Game &game) {
    this->game = &game;
    CamTextureInitialized = false;
    Clear();
}

CameraManager::CameraManager() {
}

CameraManager::~CameraManager() {
}

void CameraManager::QuickZoom(float Zoom, double Time, bool Instant) {
    this->CameraZoom = Zoom;
    if (Instant)
        RaylibCamera.zoom = Zoom;
    ZoomResetTimer = Time;
}

void CameraManager::Display() {
    if (ShaderDraw)
        BeginShaderMode(game->Shaders["pixelizer"]);
    int w = CameraRenderTexture.texture.width;
    int h = CameraRenderTexture.texture.height;
    if (uWidth == -1 || uHeight == -1 || uPixelSize == -1) {
        uWidth = GetShaderLocation(this->game->Shaders["pixelizer"], "renderWidth");
        uHeight = GetShaderLocation(this->game->Shaders["pixelizer"], "renderHeight");
        uPixelSize = GetShaderLocation(this->game->Shaders["pixelizer"], "pixelSize");
    }
    SetShaderValue(game->Shaders["pixelizer"], uWidth, &w, SHADER_UNIFORM_INT);
    SetShaderValue(game->Shaders["pixelizer"], uHeight, &h, SHADER_UNIFORM_INT);
    SetShaderValue(game->Shaders["pixelizer"], uPixelSize, &ShaderPixelPower, SHADER_UNIFORM_FLOAT);
    BeginBlendMode(BLEND_ALPHA_PREMULTIPLY);
    DrawTexturePro(CameraRenderTexture.texture, {0, 0, (float)GetScreenWidth(), (float)-GetScreenHeight()}, {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, {0,0},0, WHITE);
    EndBlendMode();
    if (ShaderDraw) {
        EndShaderMode();
        DrawText(to_string(ShaderPixelPower).c_str(), 150, 150, 50, RED);
    }
}

void CameraManager::ProcessCameraShake() {
    if (GetTime() - CameraShakeTimer >= 0.02f) {
        if (CameraShakes > 0) {
            CameraShakeOffset = {(float)GetRandomValue((int)(-50 * CameraShakeIntensity), (int)(50 * CameraShakeIntensity)), (float)GetRandomValue((int)(-50 * CameraShakeIntensity), (int)(50 * CameraShakeIntensity))};
            CameraShakes--;
        } else {
            CameraShakeOffset = {0, 0};
        }
        CameraShakeTimer = GetTime();
    }
}

void CameraManager::UpdateScreenImageSize() {
    if (CameraRenderTexture.texture.width != GetScreenWidth() || CameraRenderTexture.texture.height != GetScreenHeight()) {
        UnloadRenderTexture(CameraRenderTexture);
        CameraRenderTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    }
}

void CameraManager::BackgroundLines() {
    float ParallaxCamX = CameraPosition.x / BackgroundDepth;
    float ParallaxCamY = CameraPosition.y / BackgroundDepth;

    if (ShowLines)
    {
        for (int i = -1; i < round(GetScreenHeight() / BackgroundGridSize)+1; i++) {
            int y = (int)(ParallaxCamY / BackgroundGridSize);
            DrawLineEx({CameraPosition.x, ((y+i)*BackgroundGridSize) - ParallaxCamY + CameraPosition.y}, {(float) GetScreenWidth()+CameraPosition.x, ((y+i)*BackgroundGridSize) - ParallaxCamY + CameraPosition.y}, 7, ColorBrightness(WHITE, -0.5f));
        }

        for (int i = -1; i < round(GetScreenWidth() / BackgroundGridSize)+1; i++) {
            int x = (int)(ParallaxCamX / BackgroundGridSize);
            DrawLineEx({((x+i)*BackgroundGridSize) - ParallaxCamX + CameraPosition.x, CameraPosition.y}, {((x+i)*BackgroundGridSize) - ParallaxCamX + CameraPosition.x, (float) GetScreenHeight()+CameraPosition.y}, 7, ColorBrightness(WHITE, -0.5f));
            DrawLineEx({((x+i)*BackgroundGridSize) - ParallaxCamX + CameraPosition.x, CameraPosition.y}, {((x+i)*BackgroundGridSize) - ParallaxCamX + CameraPosition.x, (float) GetScreenHeight()+CameraPosition.y}, 3, ColorAlpha(WHITE, 0.5f));
        }

        for (int i = -1; i < round(GetScreenHeight() / BackgroundGridSize)+1; i++) {
            int y = (int)(ParallaxCamY / BackgroundGridSize);
            DrawLineEx({CameraPosition.x, ((y+i)*BackgroundGridSize) - ParallaxCamY + CameraPosition.y}, {(float) GetScreenWidth()+CameraPosition.x, ((y+i)*BackgroundGridSize) - ParallaxCamY + CameraPosition.y}, 3, ColorAlpha(WHITE, 0.5f));
        }
    } else
    {
        Texture& bg = game->Textures["bg"+to_string(BGTexture)];
        BeginShaderMode(game->Shaders["blur"]);
        int w = bg.width / (static_cast<int>(abs(sin(game->GetGameTime() / 10.0f) * 12.0f)) + 1);
        int h = bg.height / (static_cast<int>(abs(cos(game->GetGameTime() / 10.0f) * 12.0f)) + 1);
        if (uWidth2 == -1 || uHeight2 == -1) {
            uWidth2 = GetShaderLocation(this->game->Shaders["blur"], "renderWidth");
            uHeight2 = GetShaderLocation(this->game->Shaders["blur"], "renderHeight");
        }
        SetShaderValue(game->Shaders["blur"], uWidth2, &w, SHADER_UNIFORM_INT);
        SetShaderValue(game->Shaders["blur"], uHeight2, &h, SHADER_UNIFORM_INT);
        BeginBlendMode(BLEND_ALPHA_PREMULTIPLY);

        int times_x = (int) ((game->MainTileManager.MapWidth * game->MainTileManager.TileSize) / bg.width) + 1;
        int times_y = (int) ((game->MainTileManager.MapHeight * game->MainTileManager.TileSize) / bg.height) + 1;

        DrawTexturePro(bg, {0, 0, (float)bg.width*3.0f,(float)bg.height*3.0f}, {
            -ParallaxCamX + CameraPosition.x,
            -ParallaxCamY + CameraPosition.y,
            (float)bg.width*times_x,(float)bg.height*times_y}, {bg.width*(float)(times_x/2.0f),bg.height*(float)(times_y/2.0f)},0, WHITE);

        w /=2;
        h /=2;
        SetShaderValue(game->Shaders["blur"], uWidth2, &w, SHADER_UNIFORM_INT);
        SetShaderValue(game->Shaders["blur"], uHeight2, &h, SHADER_UNIFORM_INT);
        DrawTexturePro(bg, {0, 0, (float)bg.width*3.0f,(float)bg.height*3.0f}, {
            -(ParallaxCamX/2.0f) + CameraPosition.x,
            -(ParallaxCamY/2.0f) + CameraPosition.y,
            (float)bg.width*times_x,(float)bg.height*times_y}, {bg.width*(float)(times_x/2.0f),bg.height*(float)(times_y/2.0f)},0, WHITE);
        EndBlendMode();
        EndShaderMode();
    }
}

void CameraManager::ShakeCamera(float Intensity) {
    this->CameraShakeIntensity = Intensity;
    this->CameraShakeTimer = GetTime();
    this->CameraShakeOffset = {0, 0};
    this->CameraShakes = 14;
}

void CameraManager::UpdateCamera() {
    CameraTarget = {game->MainPlayer->BoundingBox.x +
                game->MainPlayer->BoundingBox.width / 2, game->MainPlayer->BoundingBox.y +
                game->MainPlayer->BoundingBox.height / 2};

    Vector2 MouseOffset = Vector2Subtract(GetMousePosition(), {static_cast<float>(GetScreenWidth()) / 2.0f, static_cast<float>(GetScreenHeight()) / 2.0f});
    MouseOffset = Vector2Divide(MouseOffset, {100,100});

    float TargetX = CameraTarget.x - CameraPositionUnaffected.x - (static_cast<float>(GetScreenWidth()) / 2.0f);
    float TargetY = CameraTarget.y - CameraPositionUnaffected.y - (static_cast<float>(GetScreenHeight()) / 2.0f);

    float ImportantVal = 20.0f * (static_cast<float>(GetFPS()) / 144.0f);
    if (ImportantVal != 0.0f) {
        CameraPositionUnaffected.x += TargetX / ImportantVal;
        CameraPositionUnaffected.y += TargetY / ImportantVal;
        CameraPosition = {CameraPositionUnaffected.x - CameraShakeOffset.x, CameraPositionUnaffected.y - CameraShakeOffset.y};
        CameraPosition = Vector2Add(CameraPosition, MouseOffset);
    }

    RaylibCamera.zoom = lerp(RaylibCamera.zoom, CameraZoom, 4.0f * game->GetGameDeltaTime());
    RaylibCamera.target = CameraPosition;//Vector2Add({((float)GetScreenWidth()/2.0f), ((float)GetScreenHeight()/2.0f)},CameraPosition);
    RaylibCamera.offset = {((float)GetScreenWidth()/2.0f) * (1-RaylibCamera.zoom), ((float)GetScreenHeight()/2.0f) * (1-RaylibCamera.zoom)};
}

void CameraManager::Begin() {
    if (ZoomResetTimer > 0)
        ZoomResetTimer -= game->GetGameDeltaTime();
    if (ZoomResetTimer <= 0)
        CameraZoom = 1.0f;

    UpdateScreenImageSize();
    ProcessCameraShake();
    UpdateCamera();
    BeginTextureMode(CameraRenderTexture);
    ClearBackground(BackgroundColor);
    BeginMode2D(RaylibCamera);
    BackgroundLines();
}

void CameraManager::End() {
    EndTextureMode();
    EndMode2D();
}

void CameraManager::Quit() {
    UnloadRenderTexture(CameraRenderTexture);
}