#ifndef POSITIONC_H
#define POSITIONC_H

#include "glm/glm.hpp"

class PositionC {
public:
    PositionC() : pos(0, 0, 0) {}
    PositionC(glm::vec3 pos) : pos(pos) {}
    glm::vec3 pos;
};

#endif
