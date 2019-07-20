#include "mc_pipeline.h"

#include "renderer/device.h"
#include "util/util.h"
#include "terrain.h"

#include "glm/glm.hpp"

#include <iostream>

MCPipeline::MCPipeline(Device& device, Terrain& terrain) : device(device), terrain(terrain) {
    
    auto poolSizes = std::vector {
        vk::DescriptorPoolSize(vk::DescriptorType::eUniformTexelBuffer, 1),
    };
    descPool = device->createDescriptorPool(vk::DescriptorPoolCreateInfo({}, 1, poolSizes.size(), poolSizes.data()));
    
    auto bindings = std::vector {
        vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformTexelBuffer, 1, vk::ShaderStageFlagBits::eCompute),
    };
    descLayout = device->createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo({}, bindings.size(), bindings.data()));
    
    
    
    descSet = device->allocateDescriptorSets(vk::DescriptorSetAllocateInfo(descPool, 1, &descLayout))[0];
    
    
    auto pcrange = vk::PushConstantRange(vk::ShaderStageFlagBits::eCompute, 0, sizeof(MCPushConstants));
    
    auto layouts = std::vector {descLayout, terrain.getDescLayout()};
    layout = device->createPipelineLayout(vk::PipelineLayoutCreateInfo({}, layouts.size(), layouts.data(), 1, &pcrange));
    
    
    
    auto computeShaderCode = Util::readFile("./shaders/marchingcubes.comp.glsl.spv");
    vk::ShaderModule computeShader = device->createShaderModule(
        vk::ShaderModuleCreateInfo({}, computeShaderCode.size() * sizeof(char), reinterpret_cast<const uint32_t*>(computeShaderCode.data())));
    
    
    auto mapEntries = std::vector {
        vk::SpecializationMapEntry(0, 0 * sizeof(int), sizeof(int)),
        vk::SpecializationMapEntry(1, 1 * sizeof(int), sizeof(int)),
        vk::SpecializationMapEntry(2, 2 * sizeof(int), sizeof(int)),
    };
    
    struct Constants {
        int x;
        int y;
        int z;
    } constants;
    constants.x = local_size.x;
    constants.y = local_size.y;
    constants.z = local_size.z;
    
    auto specConstants = vk::SpecializationInfo(mapEntries.size(), mapEntries.data(), sizeof(Constants), &constants);
    
    pipeline = device->createComputePipeline(
        nullptr,
        vk::ComputePipelineCreateInfo(
            {},
            vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eCompute, computeShader, "main", &specConstants),
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
    
    device->updateDescriptorSets({
        vk::WriteDescriptorSet(descSet, 0, 0, 1, vk::DescriptorType::eUniformTexelBuffer, nullptr, nullptr, &lookupView),
    }, {});
    
}

MCPipeline::~MCPipeline() {
    
    device->destroy(lookupView);
    
    device->destroy(pipeline);
    
    device->destroy(layout);
    
    device->destroy(descLayout);
    
    device->destroy(descPool);
    
}
