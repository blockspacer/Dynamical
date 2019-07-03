#ifndef TERRAIN_H
#define TERRAIN_H

#include "vmapp.h"

#include "glm/glm.hpp"

struct Triangle {
    glm::vec4 a,b,c;
};

class Device;

class Terrain {
public:
    Terrain(Device& device);
    ~Terrain();
    
    VmaBuffer& getTriangles() {
        return triangles;
    }
    VmaBuffer& getField() {
        return field;
    }
    VmaBuffer& getIndirect() {
        return indirect;
    }
    
private:
    Device& device;
    
    VmaBuffer field;
    VmaBuffer triangles;
    VmaBuffer indirect;
    
};

#endif
