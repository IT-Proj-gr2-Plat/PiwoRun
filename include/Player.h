#pragma once
#include "Texture.h"
#include <vector>
#include <SDL.h>

class Player {
public:
    float x = 100.f, y = 800.f;
    float vx = 0.f, vy = 0.f;
    bool onGround = false;
    std::vector<Texture*> frames;
    int curFrame = 0;
    double frameTime = 0;
    double frameDelay = 150.0;
    int width = 64, height = 64;

    int health = 3;
    int score = 0;
    float invuln = 1.5f;
    float invulnTimer = 0.0f;
    bool facingLeft = false;
    float projectileCooldown = 0.0f;

    void update(double dt, const Uint8* kb);
    void render(SDL_Renderer* r, int camX, int camY, float renderScale = 1.0f);
};