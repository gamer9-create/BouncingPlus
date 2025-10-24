//
// Created by lalit on 10/23/2025.
//

#include "CameraManager.h"
#include "Game.h"

void CameraManager::Reset() {
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
}

CameraManager::CameraManager(Game &game) {
    this->game = &game;
    CamTextureInitialized = false;
    Reset();
}

CameraManager::CameraManager() {
}

CameraManager::~CameraManager() {
}

void CameraManager::Display() {
    BeginBlendMode(BLEND_ALPHA_PREMULTIPLY);
    DrawTexturePro(CameraRenderTexture.texture, {0, 0, (float)GetScreenWidth(), (float)-GetScreenHeight()}, {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, {0,0},0, WHITE);
    EndBlendMode();
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

    for (int i = -1; i < round(GetScreenHeight() / BackgroundGridSize)+1; i++) {
        int y = (int)(ParallaxCamY / BackgroundGridSize);
        DrawLineEx({0, ((y+i)*BackgroundGridSize) - ParallaxCamY}, {(float) GetScreenWidth(), ((y+i)*BackgroundGridSize) - ParallaxCamY}, 7, ColorBrightness(WHITE, -0.5f));
    }

    for (int i = -1; i < round(GetScreenWidth() / BackgroundGridSize)+1; i++) {
        int x = (int)(ParallaxCamX / BackgroundGridSize);
        DrawLineEx({((x+i)*BackgroundGridSize) - ParallaxCamX, 0}, {((x+i)*BackgroundGridSize) - ParallaxCamX, (float) GetScreenHeight()}, 7, ColorBrightness(WHITE, -0.5f));
        DrawLineEx({((x+i)*BackgroundGridSize) - ParallaxCamX, 0}, {((x+i)*BackgroundGridSize) - ParallaxCamX, (float) GetScreenHeight()}, 3, ColorAlpha(WHITE, 0.5f));
    }

    for (int i = -1; i < round(GetScreenHeight() / BackgroundGridSize)+1; i++) {
        int y = (int)(ParallaxCamY / BackgroundGridSize);
        DrawLineEx({0, ((y+i)*BackgroundGridSize) - ParallaxCamY}, {(float) GetScreenWidth(), ((y+i)*BackgroundGridSize) - ParallaxCamY}, 3, ColorAlpha(WHITE, 0.5f));
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

    float TargetX = CameraTarget.x - CameraPositionUnaffected.x - (static_cast<float>(GetScreenWidth()) / 2.0f);
    float TargetY = CameraTarget.y - CameraPositionUnaffected.y - (static_cast<float>(GetScreenHeight()) / 2.0f);

    float ImportantVal = 20.0f * (static_cast<float>(GetFPS()) / 144.0f);
    if (ImportantVal != 0.0f) {
        CameraPositionUnaffected.x += TargetX / ImportantVal;
        CameraPositionUnaffected.y += TargetY / ImportantVal;
        CameraPosition = {CameraPositionUnaffected.x - CameraShakeOffset.x, CameraPositionUnaffected.y - CameraShakeOffset.y};
    }
}

void CameraManager::Begin(Camera2D rayCam) {
    UpdateScreenImageSize();
    ProcessCameraShake();
    UpdateCamera();
    BeginTextureMode(CameraRenderTexture);
    BeginMode2D(rayCam);
    ClearBackground(BackgroundColor);
    BackgroundLines();
}

void CameraManager::End() {
    EndTextureMode();
    EndMode2D();
}

void CameraManager::Quit() {
    UnloadRenderTexture(CameraRenderTexture);
}