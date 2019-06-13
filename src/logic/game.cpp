#include "game.h"

#include <SDL.h>

#include <iostream>

#include "renderer/renderer.h"

Game::Game() : renderer(std::make_unique<Renderer>()) {
    
}

void Game::init() {
    
    renderer->init();
    
}

void Game::start() {
    
    run();
    
}


void Game::update(float dt) {
    
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        
        if(e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)) {
            setQuitting();
        }
        
    }
    
    renderer->render();

}

void Game::quit() {
    
    renderer->quit();
    
}

Game::~Game() {
    
}
