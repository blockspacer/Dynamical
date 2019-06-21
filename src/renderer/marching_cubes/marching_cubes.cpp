#include "marching_cubes.h"

#include "renderer/device.h"

MarchingCubes::MarchingCubes(Device& device) : device(device), pipeline(device) {
    
    commandPool = device->createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, device.c_i));
    
    commandBuffer = device->allocateCommandBuffers(vk::CommandBufferAllocateInfo(commandPool, vk::CommandBufferLevel::ePrimary, 1))[0];
    
}

void MarchingCubes::compute(vk::Semaphore wait, vk::Semaphore signal) {
    
    commandBuffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    
    
    
    commandBuffer.end();
    
}

MarchingCubes::~MarchingCubes() {
    
    device->destroy(commandPool);
    
}
