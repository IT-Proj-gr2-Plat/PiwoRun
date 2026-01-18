#include "SaveData.h"
#include <vector>
#include <cstring>
#include <fstream>

void saveProgress(const SaveData& data) {
    std::vector<char> buf(sizeof(data));
    memcpy(buf.data(), &data, sizeof(data));
    for (auto& c : buf) c ^= XOR_KEY;
    std::ofstream file(SAVE_FILE, std::ios::binary | std::ios::trunc);
    if (file) {
        file.write(buf.data(), buf.size());
    }
}

SaveData loadProgress() {
    SaveData data;
    std::ifstream file(SAVE_FILE, std::ios::binary);
    if (file) {
        std::vector<char> buf(sizeof(data));
        file.read(buf.data(), sizeof(data));
        if (file.gcount() == sizeof(data)) {
            for (auto& c : buf) c ^= XOR_KEY;
            memcpy(&data, buf.data(), sizeof(data));
        }
    }
    return data;
}
