#ifndef PHYSICSC_H
#define PHYSICSC_H

#include "glm/glm.hpp"

class PhysicsC {
public:
    PhysicsC(double m) : v(0, 0, 0), m(m) {}
    glm::vec3 v;
    float m;
};

#endif
