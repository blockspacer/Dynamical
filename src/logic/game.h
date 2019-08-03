#ifndef GAME_H
#define GAME_H

#include <memory>
#include "util/game_loop.h"

#include "entt/entt.hpp"

#include "systems/systems.h"
#include "components/arguments.h"

class Game : GameLoop {
public:
    Game(int argc, char** argv);
    void init();
    void start();
    virtual void update(float dt) override;
    virtual void quit() override;
    ~Game();
    
private:
    entt::registry reg;
    Systems systems;
    
};

#endif
