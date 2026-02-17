#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <vector>

#include "AStarSearch.h"
#include "Map.h"

class Player {
public:
    Player(SDL_Renderer* renderer, Map* map)
        : renderer(renderer), map(map)
    {
    }

    void init()
    {
        SDL_Surface* surface = IMG_Load("assets/Player/player_01.png");
        player = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        surface = IMG_Load("assets/Environment/environment_03.png");
        selectionTile = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        surface = IMG_Load("assets/Environment/environment_05.png");
        pathSelected = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        // start centered on starting tile
        px = playerTileX * TILE_SIZE + TILE_SIZE * 0.5f;
        py = playerTileY * TILE_SIZE + TILE_SIZE * 0.5f;
    }

    // click sets destination tile and triggers A*
    void handleInput(int x, int y)
    {
        selectionTileX = x / TILE_SIZE;
        selectionTileY = y / TILE_SIZE;
        runAstar = true;
    }

    // dt is in seconds
    void update(float dt);

    // Respawn player back where the game begins
    void respawn();

    // Draw relative to camera
    void draw(int camX, int camY)
    {
        // selected destination marker
        if (selectionTileX >= 0 && selectionTileY >= 0) {
            SDL_Rect dest = {
                selectionTileX * TILE_SIZE - camX,
                selectionTileY * TILE_SIZE - camY,
                TILE_SIZE, TILE_SIZE
            };
            SDL_RenderCopy(renderer, selectionTile, nullptr, &dest);
        }

        // draw path tiles
        for (const Node& n : path) {
            SDL_Rect d = {
                n.x * TILE_SIZE - camX,
                n.y * TILE_SIZE - camY,
                TILE_SIZE, TILE_SIZE
            };
            SDL_RenderCopy(renderer, pathSelected, nullptr, &d);
        }

        // draw player using pixel position (centered)
        SDL_Rect p = {
            (int)(px - TILE_SIZE * 0.5f) - camX,
            (int)(py - TILE_SIZE * 0.5f) - camY,
            TILE_SIZE, TILE_SIZE
        };
        SDL_RenderCopy(renderer, player, nullptr, &p);
    }

    void clean()
    {
        if (player) SDL_DestroyTexture(player);
        if (selectionTile) SDL_DestroyTexture(selectionTile);
        if (pathSelected) SDL_DestroyTexture(pathSelected);

        player = nullptr;
        selectionTile = nullptr;
        pathSelected = nullptr;
    }

    int getCenterX() const { return (int)px; }
    int getCenterY() const { return (int)py; }

private:
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* player = nullptr;
    Map* map = nullptr;

    SDL_Texture* selectionTile = nullptr;
    SDL_Texture* pathSelected = nullptr;

    // destination tile
    int selectionTileX = -1;
    int selectionTileY = -1;

    // player tile position (for A* start) - game begins at (1,1)
    int playerTileX = 1;
    int playerTileY = 1;

    // smooth pixel position (center of sprite)
    float px = 0.0f;
    float py = 0.0f;

    // tweak this to control speed (pixels per second)
    float moveSpeed = 220.0f;

    std::vector<Node> path;
    bool runAstar = false;
};