#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <vector>

#include "Map.h"
#include "Player.h"
#include "AStarSearch.h"

class Enemy {
public:
    Enemy(SDL_Renderer* renderer, Map* map, Player* player);
    ~Enemy();

    // texturePath: spritesheet (512x64, 8 frames of 64x64)
    // spawnTileX/Y: enemy spawn in tile coords
    bool init(const char* texturePath, int spawnTileX, int spawnTileY);

    // dt in seconds
    void update(float dt);
    void draw(int camX, int camY);

    // For bullets / collision logic
    SDL_Rect getRect() const;
    bool isCollidingWithPlayer() const;

    bool isAlive() const { return alive; }
    void kill() { alive = false; }

    // Revive and return to spawn
    void resetToSpawn();

    int getCenterX() const { return (int)ex; }
    int getCenterY() const { return (int)ey; }

    void clean();

private:
    void recomputePath();
    void moveAlongPath(float dt);

    bool isBlockedTile(int tx, int ty) const;

private:
    SDL_Renderer* renderer = nullptr;
    Map* map = nullptr;
    Player* player = nullptr;

    SDL_Texture* tex = nullptr;

    // state
    bool alive = true;

    // animation
    int frameW = 64;
    int frameH = 64;
    int frameCount = 8;
    float animFps = 12.0f;
    float animTime = 0.0f;

    // spawn tile
    int spawnTileX = 0;
    int spawnTileY = 0;

    int enemyTileX = 0;
    int enemyTileY = 0;

    // smooth center position
    float ex = 0.0f;
    float ey = 0.0f;

    // movement speed
    float moveSpeed = 110.0f; // pixels/sec

    // pathing
    std::vector<Node> path;
    float repathTimer = 0.0f;
    float repathInterval = 0.25f;

    int lastPlayerTileX = -999;
    int lastPlayerTileY = -999;
};