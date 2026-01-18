#ifndef SAVEDATA_H
#define SAVEDATA_H

#include <cstdint>

struct SaveData {
    uint32_t completedLevels = 0;
    bool hasKey = false;
};

static const char* SAVE_FILE = "dodatkowy_p_3.jpg";
static const uint8_t XOR_KEY = 0xAA;

void saveProgress(const SaveData& data);
SaveData loadProgress();

#endif
