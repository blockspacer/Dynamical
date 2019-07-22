#include "marching_cubes.h"

#include "renderer/device.h"
#include "terrain.h"
#include "util/util.h"
#include "chunk.h"
#include "logic/components/chunkc.h"

#include "util/profile.h"

constexpr int timestamp_count = 2;
constexpr int total_timestamp_count = timestamp_count*NUM_FRAMES;

MarchingCubes::MarchingCubes(Device& device, Terrain& terrain) : device(device), terrain(terrain), pipeline(device, terrain) {
    
    commandPool = device->createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, device.c_i));
    
    std::vector<vk::CommandBuffer> commandBuffers = device->allocateCommandBuffers(vk::CommandBufferAllocateInfo(commandPool, vk::CommandBufferLevel::ePrimary, NUM_FRAMES));
    
    if(profiling) queryPool = device->createQueryPool(vk::QueryPoolCreateInfo({}, vk::QueryType::eTimestamp, total_timestamp_count));
    
    for(int i = 0; i<NUM_FRAMES; i++) {
        per_frame[i].commandBuffer = commandBuffers[i];
        per_frame[i].fence = device->createFence(vk::FenceCreateInfo());
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
                
                if(profiling) {
                    if(per_frame[i].chunk_count > 0) {
                        std::array<uint64_t, timestamp_count> timestamps;
                        device->getQueryPoolResults(queryPool, 0, timestamp_count, timestamps.size() * sizeof(uint64_t), timestamps.data(), sizeof(uint64_t), vk::QueryResultFlagBits::e64 | vk::QueryResultFlagBits::eWait);
                        
                        std::cout << (timestamps[timestamps.size()-1] - timestamps[0])*device.properties.limits.timestampPeriod/1000000./per_frame[i].chunk_count << std::endl;
                        std::cout << "total time " << (timestamps[timestamps.size()-1] - timestamps[0])*device.properties.limits.timestampPeriod/1000000. << std::endl;
                    }
                }
                
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
    
    
    per_frame[index].chunk_count = 0;
    
    if(reg.size<entt::tag<"modified"_hs>>() > 0) {
    
        vk::CommandBuffer commandBuffer = per_frame[index].commandBuffer;
        
        commandBuffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
        
        if(profiling) commandBuffer.resetQueryPool(queryPool, 0, timestamp_count);
        
        if(profiling) commandBuffer.writeTimestamp(vk::PipelineStageFlagBits::eTopOfPipe, queryPool, 0);
        
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline);
        
        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipeline, 0, {pipeline}, {});
        
        auto view = reg.view<ChunkC, Chunk, entt::tag<"modified"_hs>>();
        for(auto entity : view) {
            auto& chunk = view.get<ChunkC>(entity);
            auto& chonk = view.get<Chunk>(entity);
            
            commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipeline, 1, {chonk.set}, {});
            
            MCPushConstants pc {chunk.getPosition(), chunk.getCubeSize(), t};
            commandBuffer.pushConstants(pipeline, vk::ShaderStageFlagBits::eCompute, 0, sizeof(MCPushConstants), &pc);
            
            constexpr glm::ivec3 dispatchSizes(glm::ivec3(chunk_base_size/chunk_base_cube_size)/local_size);
            commandBuffer.dispatch(dispatchSizes.x, dispatchSizes.y, dispatchSizes.z);
            
            reg.remove<entt::tag<"modified"_hs>>(entity);
            reg.assign<computing>(entity, index);
            
            per_frame[index].chunk_count++;
            if(max_per_frame != 0) {
                if(per_frame[index].chunk_count >= max_per_frame) break;
            }

        }
        
        if(profiling) commandBuffer.writeTimestamp(vk::PipelineStageFlagBits::eBottomOfPipe, queryPool, 1);
        
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
    
    if(profiling) device->destroy(queryPool);
    
    device->destroy(commandPool);
    
}
