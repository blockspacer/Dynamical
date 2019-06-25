#include "game.h"

#include <iostream>

#include "renderer/renderer.h"

#include "components/input.h"

Game::Game() : renderer(std::make_unique<Renderer>()) {
    
}

void Game::init() {
    
    systems.init(reg);
    
    renderer->init(reg);
    
}

void Game::start() {
    
    run();
    
}


void Game::update(float dt) {
    
    systems.tick(reg);
    
    renderer->render(reg);
    
    Input& input = reg.ctx<Input>();
    if(input.on[Action::EXIT]) {
        setQuitting();
        input.on.set(Action::EXIT, false);
    }

}

void Game::quit() {
    
}

Game::~Game() {
    
}
