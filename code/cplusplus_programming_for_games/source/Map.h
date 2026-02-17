#pragma once
#include <SDL.h>
#include <SDL_image.h>

#define TILE_SIZE 32

class Map {
public:
    Map(SDL_Renderer* renderer) : renderer(renderer) {}

    void init() {
        SDL_Surface* surface = IMG_Load("assets/Tiles/IndustrialTile_03.png");
        backgroundTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        surface = IMG_Load("assets/Tiles/IndustrialTile_02.png");
        crateTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        // tile "3" texture (breakable)
        surface = IMG_Load("assets/Tiles/IndustrialTile_54.png");
        breakableTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    void update() {}

    void draw(int camX, int camY) {
        for (int i = 0; i < 24; i++) {
            for (int j = 0; j < 32; j++) {

                int worldX = j * TILE_SIZE;
                int worldY = i * TILE_SIZE;

                SDL_Rect dest = { worldX - camX, worldY - camY, TILE_SIZE, TILE_SIZE };

                SDL_RenderCopy(renderer, backgroundTexture, nullptr, &dest);

                if (MAP_DATA[i][j] == 1) {
                    SDL_RenderCopy(renderer, crateTexture, nullptr, &dest);
                }
                else if (MAP_DATA[i][j] == 3) {
                    SDL_RenderCopy(renderer, breakableTexture, nullptr, &dest);
                }
            }
        }
    }

    void clean() {
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyTexture(crateTexture);
        SDL_DestroyTexture(breakableTexture);
    }

    const int* operator[] (int row) const {
        return MAP_DATA[row];
    }

    // Solid tiles: 1 (crate/wall) and 3 (breakable)
    bool isWallAtPixel(int px, int py) const {
        int tx = px / TILE_SIZE;
        int ty = py / TILE_SIZE;

        if (tx < 0 || ty < 0 || tx >= 32 || ty >= 24) return true;

        int t = MAP_DATA[ty][tx];
        return (t == 1 || t == 3);
    }

    // Break tile 3 into tile 0. Returns true if a tile was broken.
    bool breakTileAtPixel(int px, int py) {
        int tx = px / TILE_SIZE;
        int ty = py / TILE_SIZE;

        if (tx < 0 || ty < 0 || tx >= 32 || ty >= 24) return false;

        if (MAP_DATA[ty][tx] == 3) {
            MAP_DATA[ty][tx] = 0;
            return true;
        }
        return false;
    }

private:
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* backgroundTexture = nullptr;
    SDL_Texture* crateTexture = nullptr;
    SDL_Texture* breakableTexture = nullptr;

	// Tilemap
	int MAP_DATA[24][32] = {
		{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
		{ 1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1 },
		{ 1,0,0,0,1,0,0,0,0,0,1,0,1,1,1,1,0,1,0,1,0,1,0,1,0,0,1,0,0,0,0,1 },
		{ 1,0,0,0,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1 },
		{ 1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1 },
		{ 1,3,1,1,1,1,1,1,1,0,1,1,1,1,1,1,0,0,0,1,0,0,0,3,0,0,1,0,1,0,0,1 },
		{ 1,0,1,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,1,1,0,1,0,0,1 },
		{ 1,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,1,0,0,1 },
		{ 1,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,1,0,0,1 },
		{ 1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,0,0,1 },
		{ 1,1,1,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
		{ 1,0,1,0,1,0,1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,0,1,0,0,0,0,1 },
		{ 1,0,1,0,1,0,1,0,1,0,0,1,3,0,0,0,0,1,0,0,1,0,1,1,1,0,1,0,0,0,0,1 },
		{ 1,0,1,0,1,0,1,0,1,0,0,1,3,0,0,0,0,1,0,0,1,0,1,0,1,0,1,0,0,0,0,1 },
		{ 1,0,1,0,1,0,1,1,1,1,0,1,0,1,1,1,1,1,0,0,1,0,1,1,1,0,1,0,0,0,0,1 },
		{ 1,0,1,0,1,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,1,0,1,0,1,0,0,0,0,1 },
		{ 1,0,1,0,1,0,1,1,1,0,0,0,0,0,1,1,1,0,1,1,1,0,1,0,1,0,1,0,0,0,0,1 },
		{ 1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,0,1,0,1,1,1,1,1,1 },
		{ 1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,1,0,1,0,0,0,0,0,0,1 },
		{ 1,0,1,0,1,0,0,0,3,0,0,0,0,1,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,1 },
		{ 1,0,1,0,1,0,0,0,0,0,0,0,0,0,3,0,1,1,0,0,1,1,1,1,1,1,1,1,1,1,0,1 },
		{ 1,0,1,0,1,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0,1,1,0,1,1,1,0,0,1,0,0,1 },
		{ 1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,1 },
		{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
	};
};