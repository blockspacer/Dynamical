#include "marching_cubes.h"

#include "renderer/device.h"
#include "renderer/terrain.h"

MarchingCubes::MarchingCubes(Device& device, Terrain& terrain) : device(device), terrain(terrain), pipeline(device, terrain) {
    
    commandPool = device->createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, device.c_i));
    
    commandBuffer = device->allocateCommandBuffers(vk::CommandBufferAllocateInfo(commandPool, vk::CommandBufferLevel::ePrimary, 1))[0];
    
}

void MarchingCubes::compute(vk::Semaphore wait, vk::Semaphore signal) {
    
    commandBuffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline);
    
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipeline, 0, {pipeline}, {});
    
    commandBuffer.dispatch(10, 4, 10);
    
    commandBuffer.end();
    
    device.compute.submit({vk::SubmitInfo(0, nullptr, nullptr, 1, &commandBuffer, 0, nullptr)}, nullptr);
    
}

MarchingCubes::~MarchingCubes() {
    
    device->destroy(commandPool);
    
}
