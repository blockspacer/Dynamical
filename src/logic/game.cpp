#include "game.h"

#include <iostream>

#include "components/inputc.h"

#include "systems/ui.h"
#include "renderer/renderer.h"

#include "systems/game_set.h"

Game::Game(int argc, char** argv) : reg(), settings(reg, argc, argv),
ui(std::make_unique<UISys>(reg)),
renderer(std::make_unique<Renderer>(reg)) {
    
    
    
}

void Game::init() {
    
    game_set = std::make_unique<GameSet>(*this);
    
    game_set->preinit();
    
    game_set->init();
    
}

void Game::start() {
    
    game_loop.run([this](float dt) {
        
        game_set->tick();
        InputC* input = reg.try_ctx<InputC>();
        if(input != nullptr && input->on[Action::EXIT]) {
            game_loop.setQuitting();
            input->on.set(Action::EXIT, false);
        }
        
    });
    
    game_set->finish();
    
}

Game::~Game() {
    
}
