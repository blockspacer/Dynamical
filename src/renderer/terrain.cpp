#include "terrain.h"

#include "device.h"

#include <iostream>

Terrain::Terrain(Device& device) : device(device) {
    
    uint32_t numTriangles = 800;
    
    VmaAllocationCreateInfo info = {};
    info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    
    bool concurrent = (device.g_i != device.c_i);
    uint32_t qfs[2] = {device.g_i, device.c_i};
    
    triangles = VmaBuffer(device, &info, vk::BufferCreateInfo({}, numTriangles * sizeof(Triangle), vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eVertexBuffer, concurrent ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive, concurrent ? 2 : 1, &qfs[0]));
    
    indirect = VmaBuffer(device, &info, vk::BufferCreateInfo({}, sizeof(VkDrawIndirectCommand), vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eIndirectBuffer, concurrent ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive, concurrent ? 2 : 1, &qfs[0]));
    
    //std::cout << triangles.buffer << std::endl;
    
}

Terrain::~Terrain() {
    
}
