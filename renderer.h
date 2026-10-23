#pragma once

#include <SDL3/SDL.h>

#include <iostream>

class PPU;

class Renderer {
   private:
    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_Texture* texture{};

   public:
    Renderer() {};
    ~Renderer();
    bool initiate();
    void render(PPU& ppu);
};