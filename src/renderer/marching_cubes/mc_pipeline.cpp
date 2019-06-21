#include "mc_pipeline.h"

#include "renderer/device.h"
#include "util/util.h"

#include <iostream>

MCPipeline::MCPipeline(Device& device) : device(device) {
    
    auto poolSizes = std::vector {
        vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 1),
        vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, 3),
        vk::DescriptorPoolSize(vk::DescriptorType::eUniformTexelBuffer, 1)
    };
    descPool = device->createDescriptorPool(vk::DescriptorPoolCreateInfo({}, 1, poolSizes.size(), poolSizes.data()));
    
    auto bindings = std::vector {
        vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eStorageBuffer, 0, vk::ShaderStageFlagBits::eCompute),
        vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eStorageBuffer, 0, vk::ShaderStageFlagBits::eCompute),
        vk::DescriptorSetLayoutBinding(2, vk::DescriptorType::eStorageBuffer, 0, vk::ShaderStageFlagBits::eCompute),
        vk::DescriptorSetLayoutBinding(3, vk::DescriptorType::eUniformBuffer, 0, vk::ShaderStageFlagBits::eCompute),
        vk::DescriptorSetLayoutBinding(4, vk::DescriptorType::eUniformTexelBuffer, 0, vk::ShaderStageFlagBits::eCompute),
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
    std::cout << lookupData.size() << std::endl;
    /*
    void* ptr = device->mapMemory(lookup->memory, lookup->offset, lookup->size);
    memcpy(ptr, bytes.constData(), lookup->size);
    win->device.logical.unmapMemory(lookup->memory);
    */
}

MCPipeline::~MCPipeline() {
    
    device->destroy(pipeline);
    
    device->destroy(layout);
    
    device->destroy(descLayout);
    
    device->destroy(descPool);
    
}
