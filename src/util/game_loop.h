#ifndef GAME_LOOP_H
#define GAME_LOOP_H

class GameLoop {
public:
    void run();
    void setQuitting();
    virtual void update(float dt) = 0;
    virtual void quit() = 0;
private:
    bool running = true;
};

#endif
