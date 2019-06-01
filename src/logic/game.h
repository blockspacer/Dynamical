#ifndef GAME_H
#define GAME_H

#include "renderer/windu.h"

class Game {
public:
    Game();
    void init();
    void run();
    ~Game();
    
private:
    Windu win;
};

#endif
