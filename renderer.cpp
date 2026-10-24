#include "renderer.h"

#include "ppu.h"

Renderer::~Renderer() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Renderer::initiate() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << SDL_GetError();
        return false;
    }

    window = SDL_CreateWindow("frametime_test", 640, 576, SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cout << SDL_GetError();
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cout << SDL_GetError();
        return false;
    }
    SDL_SetRenderLogicalPresentation(renderer, 160, 144, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 160, 144);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    if (!texture) {
        std::cout << SDL_GetError();
        return false;
    }

    return true;
}

void Renderer::render(PPU& ppu) {
    SDL_RenderClear(renderer);
    SDL_UpdateTexture(texture, nullptr, ppu.get_buffer(), 640);
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}
