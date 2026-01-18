#ifndef LEVEL_H
#define LEVEL_H

#include <SDL.h>
#include <string>
#include <vector>

class Level {
public:
    Level();
    ~Level();

    void updateBackground(float dt);
    void renderBackground(SDL_Renderer* renderer);

    void setBackgroundRepeat(bool repeat);

    void setBackgroundTexture(SDL_Texture* tex);
    void setFrameSize(int width, int height);
    void setScrollSpeed(float speed);

    void setParallax(float factor);

    void setBackgroundOffsetFromCamera(float camX, float maxCam, float dt);

    void setBackgroundMaxSpeed(float pxPerSec);

    int getFrameWidth() const;
    int getFrameHeight() const;

    int rows;
    int cols;
    std::vector<std::vector<int>> grid;
    std::string backgroundPath;
    std::vector<std::string> usedAssets;
    std::vector<std::pair<int, int>> enemyPositions;

    void toggleCell(int r, int c);
    void ensureCell(int r, int c);

    bool saveToZip(const std::string& path, const std::string& assetsDir) const;

    bool loadFromFile(const std::string& path);

private:
    SDL_Texture* bgTexture = nullptr;
    bool bgRepeat = false;
    float scrollSpeed = 0.0f;
    float parallax = 0.0f;
    float bgOffset = 0.0f;
    float bgMaxSpeed = 0.0f;
    float prevCamX = 0.0f;
    bool prevCamValid = false;
    int frameWidth = 0;
    int frameHeight = 0;
};

#endif
