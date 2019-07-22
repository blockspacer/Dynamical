#ifndef CHUNKC_H
#define CHUNKC_H

#include "glm/glm.hpp"

constexpr float chunk_base_length = 160.f;
constexpr float chunk_base_height = 40.f;
constexpr glm::vec3 chunk_base_size(chunk_base_length, chunk_base_height, chunk_base_length);
constexpr float chunk_base_cube_size = 4;
constexpr float chunk_lod_multiplier = 2;

class ChunkC {
public:
    
    glm::vec3 pos;
    int lod;
    
    glm::vec3 getPosition() {
        return pos * chunk_base_size;
    }
    
    glm::vec3 getSize() {
        return (float) std::pow(chunk_lod_multiplier, lod) * chunk_base_size;
    }
    
    float getCubeSize() {
        return std::pow(chunk_lod_multiplier, lod) * chunk_base_cube_size;
    }
    
};

#endif
