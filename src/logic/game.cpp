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
    
    for(int x = -3; x<4; x++) {
        for(int z = -3; z<4; z++) {
            auto entity = reg.create();
            ChunkC& chunk = reg.assign<ChunkC>(entity);
            chunk.pos = 40.f * glm::vec3(x, 0, z);
            chunk.cubeSize = 2.f;
            chunk.gridSize = glm::vec3(40, 32, 40);
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
