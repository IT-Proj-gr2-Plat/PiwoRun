#pragma once
#include "Texture.h"
#include "GameObjects.h"
#include <vector>
#include <SDL.h>

class Player;

class Boss {
public:
    std::vector<Texture*> frames;
    float x = 0.0f, y = 0.0f, vx = 0.0f, vy = 0.0f;
    int width = 32, height = 48;
    float hp = 10.0f;
    float invulnTimer = 0.0f;
    float invuln = 1.5f;
    bool facingLeft = false;
    int attackCount = 0;
    float attackCooldown = 0.0f;
    float attackInterval = 2.0f;
    float safeDist = 200.0f;
    bool isMoving = false;
    int currentFrame = 0;
    float animationTimer = 0.0f;

    void update(float dt, Player& player, std::vector<Projectile>& projectiles, Texture& zelazo, int levelW);
    void render(SDL_Renderer* ren, int camX, int camY, float scale);
};
