#ifndef CHUNKC_H
#define CHUNKC_H

#include "glm/glm.hpp"

namespace chunk {

    constexpr int base_cube_size = 1;
    constexpr int base_length = base_cube_size*8*8;
    constexpr int base_height = base_length;
    constexpr glm::ivec3 base_size(base_length, base_height, base_length);
    constexpr int lod_multiplier = 2;
    constexpr glm::ivec3 num_cubes = base_size/base_cube_size;
    constexpr int border = 1;
    constexpr glm::ivec3 num_values = num_cubes+1+2*border;
    
}

class ChunkC {
public:
    
    glm::ivec3 pos;
    int lod;
    
    glm::ivec3 getPosition() {
        return pos * chunk::base_size;
    }
    
    glm::ivec3 getSize() {
        return (int) std::pow(chunk::lod_multiplier, lod) * chunk::base_size;
    }
    
    int getCubeSize() {
        return std::pow(chunk::lod_multiplier, lod) * chunk::base_cube_size;
    }
    
    int getLOD() {
        return std::pow(chunk::lod_multiplier, lod);
    }
    
};

#endif
