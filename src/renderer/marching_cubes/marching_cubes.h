#ifndef MARCHING_CUBES_H
#define MARCHING_CUBES_H

#include "vulkan/vulkan.hpp"
#include "vma/vk_mem_alloc.h"

#include "mc_pipeline.h"

class Instance;
class Device;
class Swapchain;

class MarchingCubes {
public:
    MarchingCubes(Device& device);
    void compute(vk::Semaphore wait, vk::Semaphore signal);
    ~MarchingCubes();
    
private:
    vk::CommandPool commandPool;
    vk::CommandBuffer commandBuffer;
    vk::Fence fence;
    
    Device& device;
    
    MCPipeline pipeline;
    
};

#endif
