#ifndef GAME_LOOP_H
#define GAME_LOOP_H

#include <functional>

class GameLoop {
public:
    void run(std::function<void(float)> update);
    void setQuitting();
private:
    bool running = true;
};

#endif
