#include "game.h"

#include <iostream>

#include "renderer/renderer.h"

#include "components/inputc.h"

#include "components/chunkc.h"
#include "components/positionc.h"

Game::Game() {
    
}

void Game::init() {
    
    systems.init(reg);
    
    for(int x = 0; x<4; x++) {
        for(int z = 0; z<4; z++) {
            auto entity = reg.create();
            ChunkC& chonk = reg.assign<ChunkC>(entity);
            chonk.pos = 79.f * glm::vec3(x, 0, z);
            chonk.size = 0.2;
        }
    }
    
}

void Game::start() {
    
    run();
    
}

void Game::update(float dt) {
    
    systems.tick(reg);
    
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
