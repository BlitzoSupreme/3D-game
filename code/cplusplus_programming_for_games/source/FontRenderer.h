#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class FontRenderer
{
public:
    explicit FontRenderer(SDL_Renderer* sdlRenderer);

    void init();
    void clean();

    void render(const std::string& text);

    void renderAt(const std::string& text, int x, int y);

    void renderTopRight(const std::string& text, int screenWidth, int padding = 10);

private:
    SDL_Renderer* renderer = nullptr;
    TTF_Font* font = nullptr;
};