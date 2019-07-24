#ifndef CHUNKC_H
#define CHUNKC_H

#include "glm/glm.hpp"

namespace chunk {

    constexpr int base_length = 160;
    constexpr int base_height = 80;
    constexpr glm::ivec3 base_size(base_length, base_height, base_length);
    constexpr int base_cube_size = 4;
    constexpr int lod_multiplier = 2;
    constexpr glm::ivec3 num_cubes = base_size/base_cube_size;
    constexpr glm::ivec3 num_values = num_cubes+1;
    
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
    
};

#endif
