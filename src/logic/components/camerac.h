#ifndef CAMERAC_H
#define CAMERAC_H

#include "glm/glm.hpp"

#include <mutex>

class CameraC {
public:
    glm::vec3 pos;
    float yAxis;
    float xAxis;
    bool sprinting;
};

#endif
