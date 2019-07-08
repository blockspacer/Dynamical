#include "marching_cubes.h"

#include "renderer/device.h"
#include "terrain.h"
#include "util/util.h"
#include "renderer/num_frames.h"
#include "chunk.h"
#include "logic/components/chunkc.h"

MarchingCubes::MarchingCubes(Device& device, Terrain& terrain) : device(device), terrain(terrain), pipeline(device, terrain),
fences(NUM_FRAMES) {
    
    commandPool = device->createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, device.c_i));
    
    commandBuffers = device->allocateCommandBuffers(vk::CommandBufferAllocateInfo(commandPool, vk::CommandBufferLevel::ePrimary, NUM_FRAMES));
    
    for(int i = 0; i<fences.size(); i++) {
        fences[i] = device->createFence(vk::FenceCreateInfo());
    }
    
}

void MarchingCubes::compute(entt::registry& reg, uint32_t index, std::vector<vk::Semaphore> waits, std::vector<vk::Semaphore> signals) {
    
    static float t = 0;
    
    t += 0.1;
    
    
    
    reg.view<computing>().each([&](entt::entity entity, uint32_t ind) {
        
        vk::Result result = device->waitForFences(fences[ind], VK_TRUE, ind != index ? 0 : std::numeric_limits<uint64_t>::max());
        
        if(result != vk::Result::eTimeout) {
            device->resetFences({fences[ind]});
            reg.remove<computing>(entity);
            reg.assign<entt::tag<"ready"_hs>>(entity);
        }
        
    });
    
    
    if(reg.size<entt::tag<"modified"_hs>>() > 0) {
    
        vk::CommandBuffer commandBuffer = commandBuffers[index];
        
        commandBuffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
        
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline);
        
        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipeline, 0, {pipeline}, {});
        
        reg.view<ChunkC, Chunk, entt::tag<"modified"_hs>>().each([&](entt::entity entity, ChunkC& chunk, Chunk& chonk, auto) {
            
            commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipeline, 1, {chonk.set}, {});
            MCPushConstants pc {chunk.pos, t, chunk.cubeSize};
            commandBuffer.pushConstants(pipeline, vk::ShaderStageFlagBits::eCompute, 0, sizeof(MCPushConstants), &pc);
            
            commandBuffer.dispatch(chunk.gridSize.x/8, chunk.gridSize.y/4, chunk.gridSize.z/8);
            
            reg.remove<entt::tag<"modified"_hs>>(entity);
            reg.assign<computing>(entity, index);
            
        });
        
        commandBuffer.end();
        
        
        auto stages = std::vector<vk::PipelineStageFlags> {vk::PipelineStageFlagBits::eTopOfPipe};
        
        device.compute.submit({vk::SubmitInfo(
            Util::removeElement<vk::Semaphore>(waits, nullptr), waits.data(), stages.data(),
            1, &commandBuffer,
            Util::removeElement<vk::Semaphore>(signals, nullptr), signals.data()
        )}, fences[index]);
    }
    
}

MarchingCubes::~MarchingCubes() {
    
    for(int i = 0; i<fences.size(); i++) {
        device->destroy(fences[i]);
    }
    
    device->destroy(commandPool);
    
}
