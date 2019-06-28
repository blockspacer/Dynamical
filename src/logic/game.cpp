#include "game.h"

#include <iostream>

#include "renderer/renderer.h"

#include "components/inputc.h"

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
    
    InputC& input = reg.ctx<InputC>();
    if(input.on[Action::EXIT]) {
        setQuitting();
        input.on.set(Action::EXIT, false);
    }

}

void Game::quit() {
    
}

Game::~Game() {
    
}
