#ifndef GAME_H
#define GAME_H

#include <memory>

class Renderer;

class Game {
public:
    Game();
    void init();
    void run();
    ~Game();
    
private:
    std::unique_ptr<Renderer> renderer;
};

#endif
