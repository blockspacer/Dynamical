#include "main_render.h"

#include "renderer/instance.h"
#include "renderer/device.h"
#include "renderer/swapchain.h"

MainRender::MainRender(Instance& instance, Device& device, Swapchain& swap) : renderpass(device, swap), pipeline(device, swap, renderpass), instance(instance), device(device), swap(swap), commandBuffers(swap.NUM_FRAMES), fences(swap.NUM_FRAMES) {
    
    commandPool = device->createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, device.g_i));
    
    commandBuffers = device->allocateCommandBuffers(vk::CommandBufferAllocateInfo(commandPool, vk::CommandBufferLevel::ePrimary, swap.NUM_FRAMES));
    
    for(int i = 0; i < fences.size(); i++) {
        
        fences[i] = device->createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
        
    }
    
    setup();
    
    
}

void MainRender::setup() {
    
    for(int i = 0; i < commandBuffers.size(); i++) {
        
        vk::CommandBuffer command = commandBuffers[i];
        
        command.begin(vk::CommandBufferBeginInfo({}, nullptr));
        
        auto clearValues = std::vector<vk::ClearValue> {
            vk::ClearValue(vk::ClearColorValue(std::array<float, 4> { 0.2f, 0.2f, 0.2f, 1.0f })),
            vk::ClearValue(vk::ClearDepthStencilValue(1.0f, 0))};
        command.beginRenderPass(vk::RenderPassBeginInfo(renderpass, renderpass.framebuffers[i], vk::Rect2D({}, swap.extent), clearValues.size(), clearValues.data()), vk::SubpassContents::eInline);
        
        command.setViewport(0, vk::Viewport(0, 0, swap.extent.width, swap.extent.height, 0, 1));
        
        command.setScissor(0, vk::Rect2D(vk::Offset2D(), swap.extent));
        
        command.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.pipeline);
        
        command.draw(1, 1, 0, 0);
        
        command.endRenderPass();
        
        command.end();
        
    }
    
}

void MainRender::rsetup() {
    
    renderpass.setup();
    setup();
    
}

void MainRender::render(uint32_t index, vk::Semaphore wait, vk::Semaphore signal) {
    
    device->waitForFences(fences[index], VK_TRUE, 1000000);
    
    device->resetFences(fences[index]);
    
    vk::PipelineStageFlags stage = vk::PipelineStageFlagBits::eTopOfPipe;
    
    device.graphics.submit({vk::SubmitInfo(1, &wait, &stage, 1, &commandBuffers[index], 1, &signal)}, fences[index]);
    
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
