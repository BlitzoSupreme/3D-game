#include "Enemy.h"
#include <cmath>

static bool aabbOverlap(const SDL_Rect& a, const SDL_Rect& b)
{
    return (a.x < b.x + b.w) && (a.x + a.w > b.x) &&
        (a.y < b.y + b.h) && (a.y + a.h > b.y);
}

Enemy::Enemy(SDL_Renderer* renderer_, Map* map_, Player* player_)
    : renderer(renderer_), map(map_), player(player_)
{
}

Enemy::~Enemy()
{
    clean();
}

bool Enemy::init(const char* texturePath, int spawnTileX_, int spawnTileY_)
{
    spawnTileX = spawnTileX_;
    spawnTileY = spawnTileY_;

    enemyTileX = spawnTileX;
    enemyTileY = spawnTileY;

    ex = enemyTileX * TILE_SIZE + TILE_SIZE * 0.5f;
    ey = enemyTileY * TILE_SIZE + TILE_SIZE * 0.5f;

    SDL_Surface* surf = IMG_Load(texturePath);
    if (!surf) {
        SDL_Log("Enemy IMG_Load failed (%s): %s", texturePath, IMG_GetError());
        return false;
    }

    tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    if (!tex) {
        SDL_Log("Enemy CreateTextureFromSurface failed: %s", SDL_GetError());
        return false;
    }

    alive = true;
    animTime = 0.0f;
    repathTimer = 0.0f;
    path.clear();
    lastPlayerTileX = -999;
    lastPlayerTileY = -999;

    return true;
}

void Enemy::clean()
{
    if (tex) {
        SDL_DestroyTexture(tex);
        tex = nullptr;
    }
}

void Enemy::resetToSpawn()
{
    alive = true;

    enemyTileX = spawnTileX;
    enemyTileY = spawnTileY;

    ex = enemyTileX * TILE_SIZE + TILE_SIZE * 0.5f;
    ey = enemyTileY * TILE_SIZE + TILE_SIZE * 0.5f;

    path.clear();
    repathTimer = 0.0f;
    lastPlayerTileX = -999;
    lastPlayerTileY = -999;
}

bool Enemy::isBlockedTile(int tx, int ty) const
{
    
    if (tx < 0 || ty < 0 || tx >= 32 || ty >= 24) return true;

    int t = (*map)[ty][tx];
    return (t == 1 || t == 3);
}

SDL_Rect Enemy::getRect() const
{
    SDL_Rect r{
        (int)(ex - TILE_SIZE * 0.5f),
        (int)(ey - TILE_SIZE * 0.5f),
        TILE_SIZE,
        TILE_SIZE
    };
    return r;
}

bool Enemy::isCollidingWithPlayer() const
{
    if (!player) return false;

    SDL_Rect er = getRect();
    SDL_Rect pr{
        player->getCenterX() - (TILE_SIZE / 2),
        player->getCenterY() - (TILE_SIZE / 2),
        TILE_SIZE,
        TILE_SIZE
    };

    return aabbOverlap(er, pr);
}

void Enemy::recomputePath()
{
    if (!alive) return;
    if (!map || !player) return;

    int pTileX = player->getCenterX() / TILE_SIZE;
    int pTileY = player->getCenterY() / TILE_SIZE;

    // clamp to map bounds
    if (pTileX < 0) pTileX = 0;
    if (pTileY < 0) pTileY = 0;
    if (pTileX > 31) pTileX = 31;
    if (pTileY > 23) pTileY = 23;

    Node start{ enemyTileX, enemyTileY };
    Node dest{ pTileX, pTileY };

    path = aStar(*map, start, dest);

    // remove current tile if path returns it first
    if (!path.empty() && path.front().x == enemyTileX && path.front().y == enemyTileY) {
        path.erase(path.begin());
    }

    lastPlayerTileX = pTileX;
    lastPlayerTileY = pTileY;
}

void Enemy::moveAlongPath(float dt)
{
    if (path.empty()) return;

    const Node next = path.front();
    const float targetX = next.x * TILE_SIZE + TILE_SIZE * 0.5f;
    const float targetY = next.y * TILE_SIZE + TILE_SIZE * 0.5f;

    const float dx = targetX - ex;
    const float dy = targetY - ey;
    const float dist = std::sqrt(dx * dx + dy * dy);

    if (dist < 0.5f) {
        ex = targetX;
        ey = targetY;
        enemyTileX = next.x;
        enemyTileY = next.y;
        path.erase(path.begin());
        return;
    }

    const float step = moveSpeed * dt;

    if (step >= dist) {
        ex = targetX;
        ey = targetY;
        enemyTileX = next.x;
        enemyTileY = next.y;
        path.erase(path.begin());
        return;
    }

    ex += (dx / dist) * step;
    ey += (dy / dist) * step;
}

void Enemy::update(float dt)
{
    animTime += dt;

    if (!alive) return;

    // Repath periodically or if player moved tiles
    repathTimer += dt;

    int pTileX = player->getCenterX() / TILE_SIZE;
    int pTileY = player->getCenterY() / TILE_SIZE;

    if (pTileX < 0) pTileX = 0;
    if (pTileY < 0) pTileY = 0;
    if (pTileX > 31) pTileX = 31;
    if (pTileY > 23) pTileY = 23;

    if (repathTimer >= repathInterval || pTileX != lastPlayerTileX || pTileY != lastPlayerTileY) {
        repathTimer = 0.0f;
        recomputePath();
    }

    moveAlongPath(dt);
}

void Enemy::draw(int camX, int camY)
{
    if (!tex) return;
    if (!alive) return; // destroyed => not drawn

    const int frame = ((int)(animTime * animFps)) % frameCount;

    SDL_Rect src{ frame * frameW, 0, frameW, frameH };
    SDL_Rect dst{
        (int)(ex - TILE_SIZE * 0.5f) - camX,
        (int)(ey - TILE_SIZE * 0.5f) - camY,
        TILE_SIZE,
        TILE_SIZE
    };

    SDL_RenderCopy(renderer, tex, &src, &dst);
}