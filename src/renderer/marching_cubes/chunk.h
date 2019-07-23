#ifndef CHUNK_H
#define CHUNK_H

#include "vulkan/vulkan.hpp"

class Chunk {
public:
    vk::Buffer triangles = nullptr;
    uint32_t triangles_offset = 0; // in array index count
    vk::Buffer indirect = nullptr;
    uint32_t indirect_offset = 0; // in array index count
};

class ChunkBuild {
public:
    int index = 0;
    vk::DescriptorSet set = nullptr;
};

#endif
