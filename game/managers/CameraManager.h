//
// Created by lalit on 10/23/2025.
//

#ifndef BOUNCINGPLUS_CAMERA_H
#define BOUNCINGPLUS_CAMERA_H

#include <raylib.h>

class Game;

class CameraManager {
    void ProcessCameraShake();
    void UpdateScreenImageSize();
    void BackgroundLines();
    void UpdateCamera();

    bool CamTextureInitialized;

    float CameraSpeed;

    int CameraShakes;
    float CameraShakeIntensity;
    double CameraShakeTimer;
    Vector2 CameraShakeOffset;

    float BackgroundDepth;
    float BackgroundGridSize;
    Color BackgroundColor;

    int uWidth;
    int uHeight;
    int uPixelSize;

    int uWidth2;
    int uHeight2;

    bool ShowLines;

    int BGTexture;

    double ZoomResetTimer;
    public:
    RenderTexture CameraRenderTexture;
        Game* game;
        Vector2 CameraPosition;
        Vector2 CameraPositionUnaffected;
        Vector2 CameraTarget;
        Camera2D RaylibCamera;
        float CameraZoom;
        bool ShaderDraw;
        float ShaderPixelPower;
        CameraManager(Game &game);
        CameraManager();
        ~CameraManager();
        void ShakeCamera(float Intensity);
        void QuickZoom(float Zoom, double Time, bool Instant=false);
        void Display();
        void Begin();
        void End();
        void Clear();
        void Quit();
};


#endif //BOUNCINGPLUS_CAMERA_H