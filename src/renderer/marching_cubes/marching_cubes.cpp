#include "marching_cubes.h"

#include "renderer/device.h"
#include "terrain.h"
#include "util/util.h"
#include "renderer/num_frames.h"
#include "chunk.h"
#include "logic/components/chunkc.h"

constexpr glm::vec3 local_size(8, 4, 8);

MarchingCubes::MarchingCubes(Device& device, Terrain& terrain) : device(device), terrain(terrain), pipeline(device, terrain),
fences(NUM_FRAMES), fence_states(NUM_FRAMES) {
    
    commandPool = device->createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, device.c_i));
    
    commandBuffers = device->allocateCommandBuffers(vk::CommandBufferAllocateInfo(commandPool, vk::CommandBufferLevel::ePrimary, NUM_FRAMES));
    
    for(int i = 0; i<fences.size(); i++) {
        fences[i] = device->createFence(vk::FenceCreateInfo());
    }
    
}

void MarchingCubes::compute(entt::registry& reg, uint32_t index, std::vector<vk::Semaphore> waits, std::vector<vk::Semaphore> signals) {
    
    static float t = 0;
    
    t += 0.1;
    
    for(int i = 0; i<NUM_FRAMES; i++) {
        
        if(fence_states[i]) {
            
            vk::Result result = device->waitForFences(fences[i], VK_TRUE, i != index ? 0 : std::numeric_limits<uint64_t>::max());
            
            if(result != vk::Result::eTimeout) {
                device->resetFences({fences[i]});
                fence_states[i] = false;
            }
        }
        
    }
    
    reg.view<computing>().each([&](entt::entity entity, uint32_t ind) {
        
        if(!fence_states[ind]) {
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
    
        vk::CommandBuffer commandBuffer = commandBuffers[index];
        
        commandBuffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
        
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline);
        
        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipeline, 0, {pipeline}, {});
        
		auto view = reg.view<ChunkC, Chunk, entt::tag<"modified"_hs>>();
		for(auto entity : view) {
			auto& chunk = view.get<ChunkC>(entity);
			auto& chonk = view.get<Chunk>(entity);
            
            commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipeline, 1, {chonk.set}, {});
            
            const glm::vec3 gridSize = chunk.getSize();
            const float cubeSize = chunk.getCubeSize();
            
            MCPushConstants pc {chunk.getPosition(), t, glm::vec4(gridSize, cubeSize)};
            commandBuffer.pushConstants(pipeline, vk::ShaderStageFlagBits::eCompute, 0, sizeof(MCPushConstants), &pc);
            
            glm::vec3 dispatchSizes = glm::ceil(gridSize/cubeSize/local_size);
            commandBuffer.dispatch(dispatchSizes.x, dispatchSizes.y, dispatchSizes.z);
            
            reg.remove<entt::tag<"modified"_hs>>(entity);
            reg.assign<computing>(entity, index);

        }
        
        commandBuffer.end();
        
        
        auto stages = std::vector<vk::PipelineStageFlags> {vk::PipelineStageFlagBits::eTopOfPipe};
        
        device.compute.submit({vk::SubmitInfo(
            Util::removeElement<vk::Semaphore>(waits, nullptr), waits.data(), stages.data(),
            1, &commandBuffer,
            Util::removeElement<vk::Semaphore>(signals, nullptr), signals.data()
        )}, fences[index]);
        
        fence_states[index] = true;
    }
    
}

MarchingCubes::~MarchingCubes() {
    
    for(int i = 0; i<fences.size(); i++) {
        device->destroy(fences[i]);
    }
    
    device->destroy(commandPool);
    
}
