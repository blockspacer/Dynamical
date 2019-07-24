#ifndef MARCHING_CUBES_H
#define MARCHING_CUBES_H

#include "vulkan/vulkan.hpp"
#include "vma/vk_mem_alloc.h"

#include "renderer/num_frames.h"

#include "mc_pipeline.h"

#include "entt/entt.hpp"

typedef uint32_t computing;

constexpr int max_per_frame = 40;

class Device;
class Terrain;

class MarchingCubes {
public:
    MarchingCubes(Device& device, Terrain& terrain);
    void compute(entt::registry& reg, uint32_t index, std::vector<vk::Semaphore> waits, std::vector<vk::Semaphore> signals);
    ~MarchingCubes();
    
private:
    vk::CommandPool commandPool;
    
    vk::QueryPool queryPool;
    
    struct PerFrame {
        vk::CommandBuffer commandBuffer;
        vk::Fence fence;
        bool fence_state = false;
        int chunk_count = 0;
    };
    
    std::array<PerFrame, NUM_FRAMES> per_frame;
    
    
    Device& device;
    Terrain& terrain;
    
    MCPipeline pipeline;
    
};

#endif
