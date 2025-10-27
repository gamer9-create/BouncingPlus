//
// Created by lalit on 10/19/2025.
//

#include "ParticleManager.h"

#include <iostream>
#include <raymath.h>

#include "Game.h"

ParticleManager::ParticleManager() {
}

ParticleManager::ParticleManager(Game &game) {
    this->game = &game;
    particles = std::vector<Particle>();
}

void ParticleManager::ParticleEffect(ParticleData Data, float Angle, int AngleRange, int Amount) {
    for (int i = 0; i < Amount; i++) {
        Particle newParticle = {
            Data,
            Data.StartPosition,
            {0, 0},
            Data.StartVelocity + GetRandomValue(-50,50),
            GetTime(),
            Data.StartColor
        };
        newParticle.Position += {(float) GetRandomValue(-15, 15), (float) GetRandomValue(-15, 15)};
        newParticle.Data.TargetColor = ColorBrightness(Data.TargetColor, GetRandomValue(-14, 14) / 100.0f);
        newParticle.Data.Lifetime += GetRandomValue(-3.5f, 3.5f) / 10.0f;
        float RandomAngle = Angle - (AngleRange / 2) + GetRandomValue(0, AngleRange);
        float X = cos(RandomAngle * (2 * PI / 360))*100;
        float Y = sin(RandomAngle * (2 * PI / 360))*100;
        newParticle.Target = Vector2Normalize({X, Y});
        particles.push_back(newParticle);
    }
}

void ParticleManager::Reset() {
    particles.clear();
}

void ParticleManager::Update() {
    for (int i = particles.size() - 1; i >= 0; i--) {
        Particle &p = particles[i];

        double Percent = (GetTime() - p.SpawnTime) / p.Data.Lifetime;

        p.Velocity -= p.Data.VelocitySlowdown * GetFrameTime();
        if (p.Velocity <= 0)
            p.Velocity = 0;
        p.Position += p.Target * p.Velocity * GetFrameTime();
        p.ParticleColor = ColorLerp(p.ParticleColor, p.Data.TargetColor, Percent);

        DrawCircleV({p.Position.x - game->MainCameraManager.CameraPosition.x, p.Position.y - game->MainCameraManager.CameraPosition.y}, p.Data.Size, p.ParticleColor);

        if (Percent >= 1.0)
            particles.erase(particles.begin() + i);
    }
}

void ParticleManager::Quit() {
}
