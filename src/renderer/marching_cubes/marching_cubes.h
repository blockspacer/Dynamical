#ifndef MARCHING_CUBES_H
#define MARCHING_CUBES_H

#include "vulkan/vulkan.hpp"
#include "vma/vk_mem_alloc.h"

#include "renderer/num_frames.h"

#include "mc_pipeline.h"

#include "entt/entt.hpp"

#include "logic/systems/system.h"

typedef uint32_t computing;

constexpr int max_per_frame = 20;

class MarchingCubes : public System {
public:
    MarchingCubes();
    void init(entt::registry& reg) override;
    void tick(entt::registry& reg) override;
    void finish(entt::registry& reg) override;
    const char* name() override {return "Marching Cubes";};
    
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
    
    std::unique_ptr<MCPipeline> pipeline;
    
};

#endif
