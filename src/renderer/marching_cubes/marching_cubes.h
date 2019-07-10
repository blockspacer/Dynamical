#ifndef MARCHING_CUBES_H
#define MARCHING_CUBES_H

#include "vulkan/vulkan.hpp"
#include "vma/vk_mem_alloc.h"

#include "mc_pipeline.h"

#include "entt/entt.hpp"

typedef uint32_t computing;

class Device;
class Terrain;

class MarchingCubes {
public:
    MarchingCubes(Device& device, Terrain& terrain);
    void compute(entt::registry& reg, uint32_t index, std::vector<vk::Semaphore> waits, std::vector<vk::Semaphore> signals);
    ~MarchingCubes();
    
private:
    vk::CommandPool commandPool;
    std::vector<vk::CommandBuffer> commandBuffers;
    std::vector<vk::Fence> fences;
    std::vector<bool> fence_states;
    
    Device& device;
    Terrain& terrain;
    
    MCPipeline pipeline;
    
};

#endif
