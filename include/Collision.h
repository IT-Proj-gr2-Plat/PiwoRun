#pragma once
#include "Player.h"
#include "Enemy.h"
#include "Boss.h"
#include "Level.h"
#include "SaveData.h"

namespace Collision {
    void resolvePlayerCollisions(Player& player, Level& level, int cellW, int cellH, SaveData& saveData);
    void resolveEnemyCollisions(Enemy& enemy, Level& level, int cellW, int cellH);
    void resolveBossCollisions(Boss& boss, Level& level, int cellW, int cellH);
}
