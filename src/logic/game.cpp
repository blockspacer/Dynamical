#include "game.h"

#include "renderer/renderer.h"

Game::Game() : renderer(std::make_unique<Renderer>()) {
    
}

void Game::init() {
    renderer->init();
}

void Game::run() {
    
}

Game::~Game() {
    
}
