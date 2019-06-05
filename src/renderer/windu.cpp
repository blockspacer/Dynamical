#include "windu.h"

#include <iostream>
#include <vector>

#include <SDL_vulkan.h>
#include <vulkan/vulkan.hpp>

Windu::Windu() {
    
    SDL_Init(SDL_INIT_VIDEO);
    
    // be sure to initialize your SDL window with the vulkan flag
    window = SDL_CreateWindow("My App",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              1920, 1080,
                              SDL_WINDOW_VULKAN | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED);
    
    SDL_GetWindowSize(window, &width, &height);
    
}

int Windu::getHeight() {
    return height;
}

int Windu::getWidth() {
    return width;
}


Windu::~Windu() {
    
    SDL_DestroyWindow(window);

    SDL_Quit();
}
