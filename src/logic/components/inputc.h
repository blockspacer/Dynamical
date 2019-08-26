#ifndef INPUTC_H
#define INPUTC_H

#include <bitset>
#include "glm/glm.hpp"
#include "SDL.h"

enum Action : char {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    SPRINT,
    PRIMARY,
    SECONDARY,
    RESIZE,
    EXIT,
    END_ENUM
};

class InputC {
public:
    std::bitset<Action::END_ENUM> on;
    glm::ivec2 mousePos;
    glm::ivec2 mouseDiff;
	bool focused = true;
};

#endif
