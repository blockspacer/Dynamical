#ifndef CHUNK_H
#define CHUNK_H

#include "vulkan/vulkan.hpp"

class Chunk {
public:
    vk::Buffer triangles;
    uint32_t triangles_offset; // in array index count
    vk::Buffer indirect;
    uint32_t indirect_offset; // in array index count
    vk::DescriptorSet set;
};

#endif
