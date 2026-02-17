#include "FontRenderer.h"
#include "SDL_ttf.h"
#include <iostream>

FontRenderer::FontRenderer(SDL_Renderer* sdlRenderer)
{
    renderer = sdlRenderer;
}

void FontRenderer::init()
{
    if (TTF_WasInit() == 0) {
        if (TTF_Init() != 0) {
            std::cout << "TTF_Init failed: " << TTF_GetError() << std::endl;
        }
    }

    font = TTF_OpenFont("assets/Font.otf", 32);
    if (font == nullptr) {
        std::cout << "TTF_OpenFont failed: " << TTF_GetError() << std::endl;
    }
}

void FontRenderer::clean()
{
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }

    if (TTF_WasInit() != 0) {
        TTF_Quit();
    }
}

void FontRenderer::render(const std::string& text)
{
    SDL_Color colour{ 100, 100, 200, 255 };

    if (!font) return;

    SDL_Surface* textImage = TTF_RenderText_Solid(font, text.c_str(), colour);
    if (!textImage) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textImage);
    if (!texture) {
        SDL_FreeSurface(textImage);
        return;
    }

    SDL_RenderCopy(renderer, texture, nullptr, nullptr);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(textImage);
}

void FontRenderer::renderAt(const std::string& text, int x, int y)
{
    SDL_Color colour{ 120, 0, 60, 255 };

    if (!font) return;

    SDL_Surface* textImage = TTF_RenderText_Solid(font, text.c_str(), colour);
    if (!textImage) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textImage);
    if (!texture) {
        SDL_FreeSurface(textImage);
        return;
    }

    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = textImage->w;
    dst.h = textImage->h;

    SDL_RenderCopy(renderer, texture, nullptr, &dst);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(textImage);
}

void FontRenderer::renderTopRight(const std::string& text, int screenWidth, int padding)
{
    SDL_Color colour{ 255, 255, 255, 255 };

    if (!font) return;

    SDL_Surface* textImage = TTF_RenderText_Solid(font, text.c_str(), colour);
    if (!textImage) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textImage);
    if (!texture) {
        SDL_FreeSurface(textImage);
        return;
    }

    SDL_Rect dst;
    dst.w = textImage->w;
    dst.h = textImage->h;
    dst.x = screenWidth - dst.w - padding;
    dst.y = padding;

    SDL_RenderCopy(renderer, texture, nullptr, &dst);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(textImage);
}