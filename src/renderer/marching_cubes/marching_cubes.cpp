#include "marching_cubes.h"

#include "renderer/device.h"
#include "terrain.h"
#include "util/util.h"
#include "chunk.h"
#include "logic/components/chunkc.h"

constexpr int max_per_frame = 20;

MarchingCubes::MarchingCubes(Device& device, Terrain& terrain) : device(device), terrain(terrain), pipeline(device, terrain) {
    
    commandPool = device->createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, device.c_i));
    
    std::vector<vk::CommandBuffer> commandBuffers = device->allocateCommandBuffers(vk::CommandBufferAllocateInfo(commandPool, vk::CommandBufferLevel::ePrimary, NUM_FRAMES));
    
    queryPool = device->createQueryPool(vk::QueryPoolCreateInfo({}, vk::QueryType::eTimestamp, max_per_frame+1));
    
    for(int i = 0; i<NUM_FRAMES; i++) {
        per_frame[i].commandBuffer = commandBuffers[i];
        per_frame[i].fence = device->createFence(vk::FenceCreateInfo());
        per_frame[i].fence_state = false;
    }
    
}

void MarchingCubes::compute(entt::registry& reg, uint32_t index, std::vector<vk::Semaphore> waits, std::vector<vk::Semaphore> signals) {
    
    static float t = 0;
    
    t += 0.1;
    
    for(int i = 0; i<NUM_FRAMES; i++) {
        
        if(per_frame[i].fence_state) {
            
            vk::Result result = device->waitForFences(per_frame[i].fence, VK_TRUE, i != index ? 0 : std::numeric_limits<uint64_t>::max());
            
            if(result != vk::Result::eTimeout) {
                device->resetFences({per_frame[i].fence});
                per_frame[i].fence_state = false;
            }
        }
        
    }
    
    reg.view<computing>().each([&](entt::entity entity, uint32_t ind) {
        
        if(!per_frame[ind].fence_state) {
            reg.remove<computing>(entity);
            reg.assign<entt::tag<"ready"_hs>>(entity);
        }
        
    });
    
    reg.view<entt::tag<"destroying"_hs>>().each([&](entt::entity entity, auto) {
        
        if(!reg.has<computing>(entity)) {
            reg.destroy(entity);
        }
        
    });
    
    
    if(reg.size<entt::tag<"modified"_hs>>() > 0) {
    
        vk::CommandBuffer commandBuffer = per_frame[index].commandBuffer;
        
        commandBuffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
        
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline);
        
        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipeline, 0, {pipeline}, {});
        
        int i = 0;
        
        reg.view<ChunkC, Chunk, entt::tag<"modified"_hs>>().each([&](entt::entity entity, ChunkC& chunk, Chunk& chonk, auto) {
            
            commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipeline, 1, {chonk.set}, {});
            
            const float cubeSize = chunk.getCubeSize();
            
            MCPushConstants pc {chunk.getPosition(), cubeSize, t};
            commandBuffer.pushConstants(pipeline, vk::ShaderStageFlagBits::eCompute, 0, sizeof(MCPushConstants), &pc);
            
            constexpr glm::ivec3 dispatchSizes(glm::ivec3(chunk_base_size/chunk_base_cube_size)/local_size);
            commandBuffer.dispatch(dispatchSizes.x, dispatchSizes.y, dispatchSizes.z);
            
            reg.remove<entt::tag<"modified"_hs>>(entity);
            reg.assign<computing>(entity, index);
            
        });
        
        commandBuffer.end();
        
        
        auto stages = std::vector<vk::PipelineStageFlags> {vk::PipelineStageFlagBits::eTopOfPipe};
        
        device.compute.submit({vk::SubmitInfo(
            Util::removeElement<vk::Semaphore>(waits, nullptr), waits.data(), stages.data(),
            1, &commandBuffer,
            Util::removeElement<vk::Semaphore>(signals, nullptr), signals.data()
        )}, per_frame[index].fence);
        
        per_frame[index].fence_state = true;
    }
    
}

MarchingCubes::~MarchingCubes() {
    
    for(int i = 0; i<NUM_FRAMES; i++) {
        device->destroy(per_frame[i].fence);
    }
    
    device->destroy(queryPool);
    
    device->destroy(commandPool);
    
}
