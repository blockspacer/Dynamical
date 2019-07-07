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
    
    {
        auto entity = reg.create();
        ChunkC& chonk = reg.assign<ChunkC>(entity);
        chonk.pos = glm::vec3(0.f);
        chonk.lod = 4;
    }
    
    {
        auto entity = reg.create();
        ChunkC& chonk = reg.assign<ChunkC>(entity);
        chonk.pos = glm::vec3(30.f, 30.f, 30.f);
        chonk.lod = 4;
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
