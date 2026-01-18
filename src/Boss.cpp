#include "Boss.h"
#include "Player.h"
#include <cmath>

void Boss::update(float dt, Player& player, std::vector<Projectile>& projectiles, Texture& zelazo, int levelW) {
    float dx = player.x - x;
    float dist = fabs(dx);
    if (dist > safeDist + 10) {
        vx = (dx > 0 ? 50.0f : -50.0f);
    } else if (dist < safeDist - 10) {
        vx = (dx > 0 ? -50.0f : 50.0f);
    } else {
        vx = 0;
    }
    x += vx * dt;
    if (x < 0) x = 0;
    if (x > levelW - width) x = levelW - width;

    attackCooldown -= dt;
    if (attackCooldown <= 0) {
        attackCooldown = attackInterval;
        attackCount++;
        Projectile p;
        p.tex = &zelazo;
        p.x = x + width / 2.0f - p.width / 2.0f;
        p.y = y - height / 2.0f - p.height / 2.0f;
        float px = player.x + player.width / 2.0f;
        float py = player.y - player.height / 2.0f;
        float dx_attack = px - p.x;
        float dy = py - p.y;
        float dist_attack = sqrt(dx_attack * dx_attack + dy * dy);
        if (dist_attack > 0) {
            dx_attack /= dist_attack;
            dy /= dist_attack;
        }
        if (attackCount % 5 == 0) {
            p.vx = dx_attack * 400;
            p.vy = dy * 400;
            p.hasPhysics = false;
            p.lifetime = 3.0f;
        } else {
            p.vx = facingLeft ? -300.0f : 300.0f;
            p.vy = -400.0f;
            p.hasPhysics = true;
        }
        p.active = true;
        p.fromPlayer = false;
        projectiles.push_back(p);
    }

    invulnTimer -= dt;
    if (invulnTimer < 0) invulnTimer = 0;

    facingLeft = (player.x < x);
    isMoving = (fabs(vx) > 0.1f);

    animationTimer += dt;
    if (animationTimer >= 1.0f) animationTimer = 0.0f;
}

void Boss::render(SDL_Renderer* ren, int camX, int camY, float scale) {
    if (isMoving) {
        currentFrame = (int)(animationTimer * 5.0f) % frames.size();
    }
    int frame = currentFrame;
    if (frames[frame] && frames[frame]->tex) {
        SDL_Rect dst = { (int)((x - camX) * scale), (int)((y - height - camY) * scale), (int)(width * scale), (int)(height * scale) };
        SDL_RenderCopyEx(ren, frames[frame]->tex, nullptr, &dst, 0.0, nullptr, facingLeft ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
    }
}
