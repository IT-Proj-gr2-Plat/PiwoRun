#include "Collision.h"
#include <algorithm>
#include <cmath>
#include <SDL_mixer.h>

extern Mix_Chunk* globalPickSound;

namespace Collision {
    void resolvePlayerCollisions(Player& player, Level& level, int cellW, int cellH, SaveData& saveData) {
        if (cellW <= 0 || cellH <= 0) return;
        if (level.rows <= 0 || level.cols <= 0) return;

        const float eps = 0.0001f;

        // Physics: player.x is left, player.y is _feet_ (bottom).
        float px = player.x;
        float pw = static_cast<float>(player.width);
        float top = player.y - static_cast<float>(player.height);
        float ph = static_cast<float>(player.height);

        int minCol = (int)std::floor(px / cellW);
        int maxCol = (int)std::floor((px + pw - eps) / cellW);
        int minRow = (int)std::floor(top / cellH);
        int maxRow = (int)std::floor((top + ph - eps) / cellH);

        minCol = std::max(0, minCol);
        minRow = std::max(0, minRow);
        maxCol = std::min(level.cols - 1, maxCol);
        maxRow = std::min(level.rows - 1, maxRow);

        for (int r = minRow; r <= maxRow; ++r) {
            if (r < 0 || r >= (int)level.grid.size()) continue;
            for (int c = minCol; c <= maxCol; ++c) {
                if (c < 0 || c >= (int)level.grid[r].size()) continue;

                int cell = level.grid[r][c]; // 0=empty,1=solid,2=damaging,3=pickup,4=pickup,5=enemy spawn,6=pickup,7=pickup,8=heal,9=rolling pickup
                if (cell == 0 || cell == 5) continue; // non-solid and ignore enemy spawn markers

                float tx = static_cast<float>(c * cellW);
                float ty = static_cast<float>(r * cellH);

                float ix = std::min(px + pw, tx + cellW) - std::max(px, tx);
                float iy = std::min(top + ph, ty + cellH) - std::max(top, ty);

                if (ix > 0.0f && iy > 0.0f) {
                    if (cell == 3 || cell == 4 || cell == 6 || cell == 7 || cell == 8 || cell == 9 || cell == 10) {
                        if (cell == 8) {
                            player.health += 1;
                        } else if (cell == 9) {
                            player.health += 1;
                            int points = 5 + rand() % 6; // 5 to 10
                            player.score += points;
                        } else if (cell == 10) {
                            saveData.hasKey = true;
                            saveProgress(saveData);
                        } else {
                            int points = 5;
                            if (cell == 4) points = 10;
                            else if (cell == 6) points = 15;
                            else if (cell == 7) points = 20;
                            player.score += points;
                        }
                        level.grid[r][c] = 0; // remove pickup
                        if (globalPickSound) Mix_PlayChannel(-1, globalPickSound, 0);
                        continue;
                    }

                    bool isDamaging = (cell == 2);

                    // Resolve along smaller penetration (push player out)
                    if (ix < iy) {
                        // horizontal push
                        if (px + pw * 0.5f < tx + cellW * 0.5f) {
                            // push left
                            px -= ix;
                        } else {
                            // push right
                            px += ix;
                        }
                        // apply horizontal correction
                        player.x = px;
                    } else {
                        // vertical push
                        if (top + ph * 0.5f < ty + cellH * 0.5f) {
                            // collision from above -> place player on top of tile
                            top = ty - ph;
                            player.vy = 0.0f;
                            player.onGround = true;
                        } else {
                            // collision from below -> push player down (head hit)
                            top += iy;
                            if (player.vy < 0.0f) player.vy = 0.0f;
                        }
                        // apply vertical correction
                        player.y = top + ph;
                    }

                    // Handle damage
                    if (isDamaging && player.invulnTimer <= 0.0f) {
                        player.health -= 1;
                        player.invulnTimer = player.invuln;
                        if (player.health < 0) player.health = 0;
                        level.grid[r][c] = 0; // remove damaging pickup
                    }
                }
            }
        }

        // Ensure the resolved values are applied
        player.x = px;
        player.y = top + ph;
    }

    void resolveEnemyCollisions(Enemy& enemy, Level& level, int cellW, int cellH) {
        if (cellW <= 0 || cellH <= 0) return;
        if (level.rows <= 0 || level.cols <= 0) return;

        const float eps = 0.0001f;

        // Enemy physics: enemy.x is left, enemy.y is _feet_ (bottom).
        float px = enemy.x;
        float pw = static_cast<float>(enemy.width);
        float top = enemy.y - static_cast<float>(enemy.height);
        float ph = static_cast<float>(enemy.height);

        int minCol = (int)std::floor(px / cellW);
        int maxCol = (int)std::floor((px + pw - eps) / cellW);
        int minRow = (int)std::floor(top / cellH);
        int maxRow = (int)std::floor((top + ph - eps) / cellH);

        minCol = std::max(0, minCol);
        minRow = std::max(0, minRow);
        maxCol = std::min(level.cols - 1, maxCol);
        maxRow = std::min(level.rows - 1, maxRow);

        for (int r = minRow; r <= maxRow; ++r) {
            if (r < 0 || r >= (int)level.grid.size()) continue;
            for (int c = minCol; c <= maxCol; ++c) {
                if (c < 0 || c >= (int)level.grid[r].size()) continue;

                int cell = level.grid[r][c]; // 0=empty,1=solid,2=damaging,3=pickup
                if (cell != 1) continue; // only solid for enemy

                float tx = static_cast<float>(c * cellW);
                float ty = static_cast<float>(r * cellH);

                float ix = std::min(px + pw, tx + cellW) - std::max(px, tx);
                float iy = std::min(top + ph, ty + cellH) - std::max(top, ty);

                if (ix > 0.0f && iy >= 0.0f) {
                    // Resolve along smaller penetration
                    if (ix < iy) {
                        // horizontal push
                        if (px + pw * 0.5f < tx + cellW * 0.5f) {
                            // push left
                            px -= ix;
                        } else {
                            // push right
                            px += ix;
                        }
                        // apply immediate horizontal correction
                        enemy.x = px;
                        enemy.vx = -enemy.vx; // reverse direction on collision
                        if (enemy.vx < 0) enemy.facingLeft = false;
                        else enemy.facingLeft = true;
                    } else {
                        // vertical push
                        if (top + ph * 0.5f < ty + cellH * 0.5f) {
                            // collision from above -> place on top
                            top = ty - ph;
                            enemy.vy = 0.0f;
                            enemy.onGround = true;
                        } else {
                            // collision from below
                            top += iy;
                            if (enemy.vy < 0.0f) enemy.vy = 0.0f;
                        }
                        enemy.y = top + ph;
                    }
                }
            }
        }

        // Prevent falling off edges
        if (enemy.onGround && fabs(enemy.vx) > 0.1f) {
            int dir = enemy.vx > 0 ? 1 : -1;
            int checkC = dir > 0 ? (int)std::floor((enemy.x + enemy.width) / cellW) : (int)std::floor(enemy.x / cellW);
            int checkR = (int)std::floor((enemy.y + 1) / cellH);
            if (checkC >= 0 && checkC < level.cols && checkR >= 0 && checkR < level.rows) {
                if (level.grid[checkR][checkC] != 1) {
                    enemy.vx = -enemy.vx;
                    if (enemy.vx < 0) enemy.facingLeft = false;
                    else enemy.facingLeft = true;
                }
            }
        }
    }

    void resolveBossCollisions(Boss& boss, Level& level, int cellW, int cellH) {
        if (cellW <= 0 || cellH <= 0) return;
        if (level.rows <= 0 || level.cols <= 0) return;

        const float eps = 0.0001f;

        float px = boss.x;
        float pw = static_cast<float>(boss.width);
        float top = boss.y - static_cast<float>(boss.height);
        float ph = static_cast<float>(boss.height);

        int minCol = (int)std::floor(px / cellW);
        int maxCol = (int)std::floor((px + pw - eps) / cellW);
        int minRow = (int)std::floor(top / cellH);
        int maxRow = (int)std::floor((top + ph - eps) / cellH);

        minCol = std::max(0, minCol);
        minRow = std::max(0, minRow);
        maxCol = std::min(level.cols - 1, maxCol);
        maxRow = std::min(level.rows - 1, maxRow);

        for (int r = minRow; r <= maxRow; ++r) {
            if (r < 0 || r >= (int)level.grid.size()) continue;
            for (int c = minCol; c <= maxCol; ++c) {
                if (c < 0 || c >= (int)level.grid[r].size()) continue;

                int cell = level.grid[r][c];
                if (cell != 1) continue; // only solid

                float tx = static_cast<float>(c * cellW);
                float ty = static_cast<float>(r * cellH);

                float ix = std::min(px + pw, tx + cellW) - std::max(px, tx);
                float iy = std::min(top + ph, ty + cellH) - std::max(top, ty);

                if (ix > 0.0f && iy >= 0.0f) {
                    if (ix < iy) {
                        if (px + pw * 0.5f < tx + cellW * 0.5f) {
                            px -= ix;
                        } else {
                            px += ix;
                        }
                        boss.x = px;
                        boss.vx = -boss.vx;
                    } else {
                        if (top + ph * 0.5f < ty + cellH * 0.5f) {
                            // collision from above -> place on top
                            top = ty - ph;
                            boss.vy = 0.0f;
                        } else {
                            // collision from below
                            top += iy;
                            if (boss.vy < 0.0f) boss.vy = 0.0f;
                        }
                        boss.y = top + ph;
                    }
                }
            }
        }
    }
}
