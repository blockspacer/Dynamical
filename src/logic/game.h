#ifndef GAME_H
#define GAME_H

#include <memory>
#include "util/game_loop.h"

#include "entt/entt.hpp"

#include "systems/systems.h"

class Renderer;

class Game : GameLoop {
public:
    Game();
    void init();
    void start();
    virtual void update(float dt) override;
    virtual void quit() override;
    ~Game();
    
private:
    entt::registry reg;
    
    Systems systems;
    
    std::unique_ptr<Renderer> renderer;
    
};

#endif
