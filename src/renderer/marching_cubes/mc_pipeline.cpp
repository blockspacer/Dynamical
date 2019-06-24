#include "mc_pipeline.h"

#include "renderer/device.h"
#include "util/util.h"
#include "renderer/terrain.h"

#include <iostream>

MCPipeline::MCPipeline(Device& device, Terrain& terrain) : device(device), terrain(terrain) {
    
    auto poolSizes = std::vector {
        vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 1),
        vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, 3),
        vk::DescriptorPoolSize(vk::DescriptorType::eUniformTexelBuffer, 1)
    };
    descPool = device->createDescriptorPool(vk::DescriptorPoolCreateInfo({}, 1, poolSizes.size(), poolSizes.data()));
    
    auto bindings = std::vector {
        vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute),
        //vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute),
        vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute),
        //vk::DescriptorSetLayoutBinding(2, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eCompute),
        vk::DescriptorSetLayoutBinding(2, vk::DescriptorType::eUniformTexelBuffer, 1, vk::ShaderStageFlagBits::eCompute),
    };
    descLayout = device->createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo({}, bindings.size(), bindings.data()));
    
    descSet = device->allocateDescriptorSets(vk::DescriptorSetAllocateInfo(descPool, 1, &descLayout))[0];
    
    layout = device->createPipelineLayout(vk::PipelineLayoutCreateInfo({}, 1, &descLayout));
    
    auto computeShaderCode = Util::readFile("./shaders/marchingcubes.comp.glsl.spv");
    vk::ShaderModule computeShader = device->createShaderModule(
        vk::ShaderModuleCreateInfo({}, computeShaderCode.size() * sizeof(char), reinterpret_cast<const uint32_t*>(computeShaderCode.data())));
    
    pipeline = device->createComputePipeline(
        nullptr,
        vk::ComputePipelineCreateInfo(
            {},
            vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eCompute, computeShader, "main"),
            layout
        )
    );
    
    device->destroy(computeShader);
    
    auto lookupData = Util::readFile("./resources/mclookup.bin");
    
    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    lookupBuffer = VmaBuffer(device, &allocInfo, vk::BufferCreateInfo(
        {}, lookupData.size() * sizeof(char), vk::BufferUsageFlagBits::eUniformTexelBuffer,
        vk::SharingMode::eExclusive, 1, &device.c_i
    ));
    
    void* ptr = device->mapMemory(lookupBuffer.memory, lookupBuffer.offset, lookupBuffer.size);
    memcpy(ptr, lookupData.data(), lookupBuffer.size);
    device->unmapMemory(lookupBuffer.memory);
    
    lookupView = device->createBufferView(vk::BufferViewCreateInfo({}, lookupBuffer, vk::Format::eR8Sint, 0, lookupBuffer.size));
    
    auto triangles = vk::DescriptorBufferInfo(terrain.getTriangles(), 0, terrain.getTriangles().size);
    auto indirect = vk::DescriptorBufferInfo(terrain.getIndirect(), 0, terrain.getIndirect().size);
    
    device->updateDescriptorSets({
        vk::WriteDescriptorSet(descSet, 0, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &triangles, nullptr),
        vk::WriteDescriptorSet(descSet, 1, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &indirect, nullptr),
        vk::WriteDescriptorSet(descSet, 2, 0, 1, vk::DescriptorType::eUniformTexelBuffer, nullptr, nullptr, &lookupView),
    }, {});
    
}

MCPipeline::~MCPipeline() {
    
    device->destroy(lookupView);
    
    device->destroy(pipeline);
    
    device->destroy(layout);
    
    device->destroy(descLayout);
    
    device->destroy(descPool);
    
}
