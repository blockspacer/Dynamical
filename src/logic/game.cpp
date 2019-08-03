#include "game.h"

#include <iostream>

#include "renderer/renderer.h"

#include "components/inputc.h"

#include "components/chunkc.h"
#include "components/positionc.h"

Game::Game(int argc, char** argv) : systems(reg) {
    auto& args = reg.set<Arguments>();
    args.argc = argc;
    args.argv = argv;
}

void Game::init() {
    systems.preinit();
    
    systems.init();
    
}

void Game::start() {
    
    run();
    
}

void Game::update(float dt) {
    
    systems.tick();
    InputC& input = reg.ctx<InputC>();
    if(input.on[Action::EXIT]) {
        setQuitting();
        input.on.set(Action::EXIT, false);
    }

}

void Game::quit() {
    
    systems.finish();
    
}

Game::~Game() {
    
}
