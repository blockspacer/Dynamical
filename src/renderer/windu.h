#ifndef WINDU_H
#define WINDU_H

#include <SDL.h>

class Windu {
public:
    Windu();
    void init();
    ~Windu();
    operator SDL_Window*() { return window; }
    int getWidth();
    int getHeight();
    
private:
    int width;
    int height;
    SDL_Window* window;
};
   
#endif
