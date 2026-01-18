#pragma once
#include "Texture.h"

struct Projectile {
    float x, y, vx, vy;
    Texture* tex;
    bool active;
    bool fromPlayer = false;
    int width = 16, height = 16;
    bool hasPhysics = true;
    float lifetime = 0.0f;
};

struct Blood {
    float x, y, vx, vy;
    int lifetime;
};
