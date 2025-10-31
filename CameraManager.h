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

    RenderTexture CameraRenderTexture;
    bool CamTextureInitialized;

    float CameraSpeed;

    int CameraShakes;
    float CameraShakeIntensity;
    double CameraShakeTimer;
    Vector2 CameraShakeOffset;
    Vector2 CameraPositionUnaffected;

    float BackgroundDepth;
    float BackgroundGridSize;
    Color BackgroundColor;

    double ZoomResetTimer;
    public:
        Game* game;
        Vector2 CameraPosition;
        Vector2 CameraTarget;
        float CameraZoom;
        CameraManager(Game &game);
        CameraManager();
        ~CameraManager();
        void ShakeCamera(float Intensity);
        void QuickZoom(float Zoom, double Time);
        void Display();
        void Begin(Camera2D rayCam);
        void End();
        void Reset();
        void Quit();
};


#endif //BOUNCINGPLUS_CAMERA_H