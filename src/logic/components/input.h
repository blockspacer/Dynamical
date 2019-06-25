#ifndef INPUT_H
#define INPUT_H

#include <bitset>
#include "glm/glm.hpp"
#include "SDL.h"

enum Action : char {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    PRIMARY,
    SECONDARY,
    RESIZE,
    EXIT,
    END_ENUM
};

class Input {
public:
    std::bitset<Action::END_ENUM> on;
    glm::ivec2 mousePos;
};

#endif
