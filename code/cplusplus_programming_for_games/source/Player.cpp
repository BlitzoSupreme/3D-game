#include "Player.h"
#include <cmath>

void Player::update(float dt)
{
    // No destination chosen yet
    if (selectionTileX < 0 || selectionTileY < 0) return;

    // Run A* only when clicked
    if (runAstar) {
        Node start{ playerTileX, playerTileY };
        Node dest{ selectionTileX, selectionTileY };

        path = aStar(*map, start, dest);
        runAstar = false;

        // If the path includes the current tile first, remove it
        if (!path.empty() && path.front().x == playerTileX && path.front().y == playerTileY) {
            path.erase(path.begin());
        }
    }

    if (path.empty()) return;

    // Move toward next tile center
    const Node next = path.front();
    const float targetX = next.x * TILE_SIZE + TILE_SIZE * 0.5f;
    const float targetY = next.y * TILE_SIZE + TILE_SIZE * 0.5f;

    const float dx = targetX - px;
    const float dy = targetY - py;
    const float dist = std::sqrt(dx * dx + dy * dy);

    // Snap when very close
    if (dist < 0.5f) {
        px = targetX;
        py = targetY;

        playerTileX = next.x;
        playerTileY = next.y;

        path.erase(path.begin());
        return;
    }

    // Step by pixels/sec
    const float step = moveSpeed * dt;

    if (step >= dist) {
        px = targetX;
        py = targetY;

        playerTileX = next.x;
        playerTileY = next.y;

        path.erase(path.begin());
    }
    else {
        px += (dx / dist) * step;
        py += (dy / dist) * step;
    }
}

void Player::respawn()
{
    // Game begins at tile (1,1)
    const int startTileX = 1;
    const int startTileY = 1;

    playerTileX = startTileX;
    playerTileY = startTileY;

    // center on tile
    px = playerTileX * TILE_SIZE + TILE_SIZE * 0.5f;
    py = playerTileY * TILE_SIZE + TILE_SIZE * 0.5f;

    // clear selection/path so player stops moving after respawn
    selectionTileX = -1;
    selectionTileY = -1;

    path.clear();
    runAstar = false;
}