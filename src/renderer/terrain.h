#ifndef TERRAIN_H
#define TERRAIN_H

#include "vmapp.h"

struct vec4 {
    float x, y, z, w;
};

struct Triangle {
    vec4 a,b,c;
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
