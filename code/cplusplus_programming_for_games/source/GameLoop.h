#pragma once

#include <SDL.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include <SDL_mixer.h>

#include "Map.h"
#include "Player.h"
#include "Enemy.h"
#include "FontRenderer.h"

struct Bullet {
    float x = 0, y = 0;
    float vx = 0, vy = 0;
    bool alive = true;
};

class GameLoop {
public:
    GameLoop();

    void init();
    bool handleInput();
    void update();
    void draw();
    void clean();

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    Map* map = nullptr;
    Player* player = nullptr;

    //multipul enemies
    std::vector<Enemy*> enemies;
    std::vector<float> enemyRespawnTimers;
    int maxEnemies = 5;
    float enemyRespawnDelay = 3.0f;

    // UI
    FontRenderer* font = nullptr;
    int score = 0;

    // Music
    Mix_Music* bgm = nullptr;
    int musicVolume = MIX_MAX_VOLUME / 2;
    bool musicPaused = false;

    bool initAudio();
    void loadAndPlayMusic(const std::string& path);
    void toggleMusicPause();
    void changeMusicVolume(int delta);

    std::vector<Bullet> bullets;
    float bulletSpeed = 600.0f;

    Uint64 lastCounter = 0;

    float camX = 0.0f;
    float camY = 0.0f;
    float zoom = 1.25f;

    int windowW = 1024;
    int windowH = 768;
};