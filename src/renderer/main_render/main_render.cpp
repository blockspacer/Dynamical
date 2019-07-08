#include "main_render.h"

#include "renderer/instance.h"
#include "renderer/device.h"
#include "renderer/swapchain.h"
#include "renderer/camera.h"
#include "renderer/marching_cubes/terrain.h"
#include "util/util.h"
#include "renderer/num_frames.h"
#include "renderer/marching_cubes/chunk.h"
#include "logic/components/chunkc.h"

MainRender::MainRender(Instance& instance, Device& device, Swapchain& swap, Camera& camera, Terrain& terrain) : renderpass(device, swap), pipeline(device, swap, renderpass), instance(instance), device(device), swap(swap), camera(camera), terrain(terrain),
commandBuffers(NUM_FRAMES), fences(NUM_FRAMES), ubos(NUM_FRAMES), uboPointers(NUM_FRAMES) {
    
    commandPool = device->createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, device.g_i));
    
    commandBuffers = device->allocateCommandBuffers(vk::CommandBufferAllocateInfo(commandPool, vk::CommandBufferLevel::ePrimary, NUM_FRAMES));
    
    for(int i = 0; i < fences.size(); i++) {
        
        fences[i] = device->createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
        
    }
    
    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    
    for(int i = 0; i < ubos.size(); i++) {
        
        ubos[i] = VmaBuffer(device, &allocInfo, vk::BufferCreateInfo({}, sizeof(UBO), vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive, 1, &device.g_i));
        
        VmaAllocationInfo inf;
        vmaGetAllocationInfo(device, ubos[i].allocation, &inf);
        void* r = inf.pMappedData;
        uboPointers[i] = static_cast<UBO*> (inf.pMappedData);
        
        auto bufInfo = vk::DescriptorBufferInfo(ubos[i], 0, ubos[i].size);
        
        device->updateDescriptorSets({
            vk::WriteDescriptorSet(pipeline.descSets[i], 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &bufInfo, nullptr)
        }, {});
        
    }
    
    setup();
    
}

void MainRender::setup() {
    
}

void MainRender::rsetup() {
    
    renderpass.setup();
    setup();
    
}

void MainRender::render(entt::registry& reg, uint32_t index, std::vector<vk::Semaphore> waits, std::vector<vk::Semaphore> signals) {
    
    device->waitForFences(fences[index], VK_TRUE, std::numeric_limits<uint64_t>::max());
    
    device->resetFences(fences[index]);
    
    uboPointers[index]->viewproj = camera.getViewProjection();
    
    vk::CommandBuffer command = commandBuffers[index];
        
    command.begin(vk::CommandBufferBeginInfo({}, nullptr));
    
    auto clearValues = std::vector<vk::ClearValue> {
        vk::ClearValue(vk::ClearColorValue(std::array<float, 4> { 0.2f, 0.2f, 0.2f, 1.0f })),
        vk::ClearValue(vk::ClearDepthStencilValue(1.0f, 0))};
    command.beginRenderPass(vk::RenderPassBeginInfo(renderpass, renderpass.framebuffers[index], vk::Rect2D({}, swap.extent), clearValues.size(), clearValues.data()), vk::SubpassContents::eInline);
    
    command.setViewport(0, vk::Viewport(0, 0, swap.extent.width, swap.extent.height, 0, 1));
    
    command.setScissor(0, vk::Rect2D(vk::Offset2D(), swap.extent));
    
    command.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
    
    command.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline, 0, {pipeline.descSets[index]}, {});
    
    reg.view<Chunk, entt::tag<"ready"_hs>>().each([&](Chunk& chonk, auto) {
        
        command.bindVertexBuffers(0, {chonk.triangles}, {chonk.triangles_offset * sizeof(Triangle)});
        
        command.drawIndirect(chonk.indirect, chonk.indirect_offset * sizeof(vk::DrawIndirectCommand), 1, 0);
        
    });
    
    command.endRenderPass();
    
    command.end();
    
    
    auto stages = std::vector<vk::PipelineStageFlags> {vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTopOfPipe};
    
    device.graphics.submit({vk::SubmitInfo(
        Util::removeElement<vk::Semaphore>(waits, nullptr), waits.data(), stages.data(),
        1, &commandBuffers[index],
        Util::removeElement<vk::Semaphore>(signals, nullptr), signals.data()
    )}, fences[index]);
    
}

void MainRender::cleanup() {
    
    device->resetCommandPool(commandPool, {});
    
}

void MainRender::rcleanup() {
    
    cleanup();
    renderpass.cleanup();
    
}

void MainRender::reset() {
    cleanup();
    setup();
}

MainRender::~MainRender() {
    
    cleanup();
    
    for(int i = 0; i < commandBuffers.size(); i++) {
        
        device->destroy(fences[i]);
        
    }
    
    device->destroy(commandPool);
    
}
