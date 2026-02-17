#include "GameLoop.h"
#include <algorithm>
#include <cmath>
#include <SDL_mixer.h>

GameLoop::GameLoop()
{
}

static float clampf(float v, float lo, float hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static bool aabbOverlap(const SDL_Rect& a, const SDL_Rect& b)
{
    return (a.x < b.x + b.w) && (a.x + a.w > b.x) &&
        (a.y < b.y + a.h) && (a.y + a.h > b.y);
}

void GameLoop::init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return;
    }

    window = SDL_CreateWindow(
        "Game",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        1024, 768,
        SDL_WINDOW_SHOWN
    );

    SDL_GetWindowSize(window, &windowW, &windowH);

    // Create renderer WITH VSYNC
    renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    map = new Map(this->renderer);
    map->init();

    player = new Player(this->renderer, map);
    player->init();

    //inits all the enemies
    enemies.clear();
    enemyRespawnTimers.clear();
    enemies.reserve(maxEnemies);
    enemyRespawnTimers.reserve(maxEnemies);

    // spawn tiles
    const int spawns[5][2] = {
        {30, 22},
        {28,  2},
        { 2, 20},
        {15, 12},
        {25,  8}
    };

    for (int i = 0; i < maxEnemies; i++) {
        Enemy* e = new Enemy(this->renderer, map, player);
        e->init("assets/ENEMY.png", spawns[i][0], spawns[i][1]);
        enemies.push_back(e);
        enemyRespawnTimers.push_back(0.0f);
    }

    // Font UI
    font = new FontRenderer(renderer);
    font->init();

    lastCounter = SDL_GetPerformanceCounter();

    if (initAudio()) {
        loadAndPlayMusic("assets/Sounds/BackgroundMusic.ogg");
    }
}

bool GameLoop::handleInput()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            return false;
        }

        if (e.type == SDL_MOUSEBUTTONDOWN) {
            int mx, my;
            SDL_GetMouseState(&mx, &my);

            // Convert screen mouse -> world mouse (camera + zoom)
            float worldMx = camX + (float)mx / zoom;
            float worldMy = camY + (float)my / zoom;

            // Left click = set A* destination (world coords)
            if (e.button.button == SDL_BUTTON_LEFT) {
                player->handleInput((int)worldMx, (int)worldMy);
            }

            // Right click = shoot toward mouse (world coords)
            if (e.button.button == SDL_BUTTON_RIGHT) {
                float px = (float)player->getCenterX();
                float py = (float)player->getCenterY();

                float dx = worldMx - px;
                float dy = worldMy - py;
                float len = std::sqrt(dx * dx + dy * dy);

                if (len > 0.001f) {
                    dx /= len;
                    dy /= len;

                    Bullet b;
                    b.x = px;
                    b.y = py;
                    b.vx = dx * bulletSpeed;
                    b.vy = dy * bulletSpeed;
                    b.alive = true;

                    bullets.push_back(b);
                }
            }
        }
    }

    return true;
}

void GameLoop::update()
{
    // Compute real dt (seconds)
    Uint64 now = SDL_GetPerformanceCounter();
    float dt = (float)((now - lastCounter) / (double)SDL_GetPerformanceFrequency());
    lastCounter = now;

    // Clamp dt
    if (dt > 0.05f) dt = 0.05f;

    map->update();
    player->update(dt);

    // update enemies and handle respawn
    for (int i = 0; i < (int)enemies.size(); i++) {
        Enemy* e = enemies[i];
        if (!e) continue;

        if (e->isAlive()) {
            e->update(dt);
        }
        else {
            if (enemyRespawnTimers[i] > 0.0f) {
                enemyRespawnTimers[i] -= dt;
                if (enemyRespawnTimers[i] <= 0.0f) {
                    enemyRespawnTimers[i] = 0.0f;
                    e->resetToSpawn(); // revive
                }
            }
        }
    }

    // Center camera on player
    float px = (float)player->getCenterX();
    float py = (float)player->getCenterY();

    camX = px - (windowW * 0.5f) / zoom;
    camY = py - (windowH * 0.5f) / zoom;

    // Clamp camera to map bounds
    float mapW = 32.0f * TILE_SIZE;
    float mapH = 24.0f * TILE_SIZE;

    float maxX = mapW - (windowW / zoom);
    float maxY = mapH - (windowH / zoom);

    if (maxX < 0.0f) maxX = 0.0f;
    if (maxY < 0.0f) maxY = 0.0f;

    camX = clampf(camX, 0.0f, maxX);
    camY = clampf(camY, 0.0f, maxY);

    //update bullets
    for (auto& b : bullets) {
        if (!b.alive) continue;

        b.x += b.vx * dt;
        b.y += b.vy * dt;

        // Break tile "3" into "0" and gain score
        if (map->breakTileAtPixel((int)b.x, (int)b.y)) {
            b.alive = false;
            score += 1;
        }
        // If not breakable, collide with solid tiles (1 or 3)
        else if (map->isWallAtPixel((int)b.x, (int)b.y)) {
            b.alive = false;
        }
    }


    for (auto& b : bullets) {
        if (!b.alive) continue;

        // Move bullet
        b.x += b.vx * dt;
        b.y += b.vy * dt;

        // Bullet hitbox (make slightly bigger to avoid "tunneling")
        SDL_Rect br{ (int)b.x - 6, (int)b.y - 6, 12, 12 };

        // Check enemy hit 
        bool hitEnemy = false;
        for (int i = 0; i < (int)enemies.size(); i++) {
            Enemy* e = enemies[i];
            if (!e || !e->isAlive()) continue;

            if (aabbOverlap(br, e->getRect())) {
                b.alive = false;
                e->kill();
                enemyRespawnTimers[i] = enemyRespawnDelay; // 3 seconds
                score += 1; // +1 for kill

                hitEnemy = true;
                break;
            }
        }
        if (hitEnemy) continue;

        //check map collisions
        if (map->breakTileAtPixel((int)b.x, (int)b.y)) {
            b.alive = false;
            score += 1;
        }
        else if (map->isWallAtPixel((int)b.x, (int)b.y)) {
            b.alive = false;
        }
    }

    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return !b.alive; }),
        bullets.end()
    );

    //if enemy touches player, reset player back to start
    for (int i = 0; i < (int)enemies.size(); i++) {
        Enemy* e = enemies[i];
        if (!e || !e->isAlive()) continue;

        if (e->isCollidingWithPlayer()) {
            player->respawn();

            // resets enemy to prevent spawn camping
            e->resetToSpawn();
            enemyRespawnTimers[i] = 0.0f;
            break;
        }
    }
}

void GameLoop::draw()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Zoom the world rendering
    SDL_RenderSetScale(renderer, zoom, zoom);

    // Draw world relative to camera
    map->draw((int)camX, (int)camY);

    // Draw enemies
    for (auto* e : enemies) {
        if (e) e->draw((int)camX, (int)camY);
    }

    // Draw player
    player->draw((int)camX, (int)camY);

    // Draw bullets relative to camera
    SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
    for (auto& b : bullets) {
        SDL_Rect r{ (int)(b.x - camX) - 2, (int)(b.y - camY) - 2, 4, 4 };
        SDL_RenderFillRect(renderer, &r);
    }

    // Reset scale for UI
    SDL_RenderSetScale(renderer, 1.0f, 1.0f);

    // Draw score in top-right
    if (font) {
        std::string scoreText = "SCORE=" + std::to_string(score);
        font->renderTopRight(scoreText, windowW, 10);
    }

    SDL_RenderPresent(renderer);
}

bool GameLoop::initAudio()
{
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("Mix_OpenAudio failed: %s", Mix_GetError());
        return false;
    }

    Mix_AllocateChannels(16);
    Mix_VolumeMusic(musicVolume);
    return true;
}

void GameLoop::loadAndPlayMusic(const std::string& path)
{
    if (bgm) {
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
        bgm = nullptr;
    }

    bgm = Mix_LoadMUS(path.c_str());
    if (!bgm) {
        SDL_Log("Mix_LoadMUS failed (%s): %s", path.c_str(), Mix_GetError());
        return;
    }

    if (Mix_PlayMusic(bgm, -1) == -1) {
        SDL_Log("Mix_PlayMusic failed: %s", Mix_GetError());
    }
    else {
        musicPaused = false;
        Mix_VolumeMusic(musicVolume);
    }
}

void GameLoop::toggleMusicPause()
{
    if (!Mix_PlayingMusic()) return;

    if (musicPaused) {
        Mix_ResumeMusic();
        musicPaused = false;
    }
    else {
        Mix_PauseMusic();
        musicPaused = true;
    }
}

void GameLoop::changeMusicVolume(int delta)
{
    musicVolume += delta;
    if (musicVolume < 0) musicVolume = 0;
    if (musicVolume > MIX_MAX_VOLUME) musicVolume = MIX_MAX_VOLUME;

    Mix_VolumeMusic(musicVolume);
}

void GameLoop::clean()
{
    if (map) map->clean();
    if (player) player->clean();
    if (font) font->clean();

    // Clean up enemies
    for (auto* e : enemies) {
        if (e) {
            e->clean();
            delete e;
        }
    }
    enemies.clear();
    enemyRespawnTimers.clear();

    delete player;
    delete map;
    delete font;

    player = nullptr;
    map = nullptr;
    font = nullptr;

    if (bgm) {
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
        bgm = nullptr;
    }
    Mix_CloseAudio();
    Mix_Quit();

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);

    renderer = nullptr;
    window = nullptr;

    SDL_Quit();
}