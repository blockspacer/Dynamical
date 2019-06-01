#include "windu.h"

#include <SDL2/SDL.h>

#include <iostream>

Windu::Windu() {
    
    SDL_Init(SDL_INIT_VIDEO);
    
    // be sure to initialize your SDL window with the vulkan flag
    window = SDL_CreateWindow("My App",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              1920, 1080,
                              SDL_WINDOW_VULKAN | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED);
    
}

void Windu::init() {
    
}

Windu::~Windu() {
    
    SDL_DestroyWindow(window);

    SDL_Quit();
}
