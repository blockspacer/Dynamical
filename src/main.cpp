#include "logic/game.h"

#include <memory>

int main(int argc, char **argv) {
    
    auto game = std::make_unique<Game>();
    
    game->init();
    
    game->start();
    
    return 0;
    
}
