#include "ubo_descriptor.h"

#include "renderer/device.h"

#include "renderer/num_frames.h"

#include "util/util.h"

UBODescriptor::UBODescriptor(Device& device) : device(device) {
    
    {
        auto poolSizes = std::vector {
            vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, NUM_FRAMES),
        };
        descPool = device->createDescriptorPool(vk::DescriptorPoolCreateInfo({}, NUM_FRAMES, poolSizes.size(), poolSizes.data()));
    }
    
    {
        auto bindings = std::vector {
            vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
        };
        descLayout = device->createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo({}, bindings.size(), bindings.data()));
        
        std::vector<vk::DescriptorSetLayout> layouts = Util::nTimes(NUM_FRAMES, descLayout);
        descSets = device->allocateDescriptorSets(vk::DescriptorSetAllocateInfo(descPool, NUM_FRAMES, layouts.data()));
        
        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
        allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
        
        for(int i = 0; i < ubos.size(); i++) {
            
            ubos[i] = VmaBuffer(device, &allocInfo, vk::BufferCreateInfo({}, sizeof(UBO), vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive, 1, &device.g_i));
            
            VmaAllocationInfo inf;
            vmaGetAllocationInfo(device, ubos[i].allocation, &inf);
            pointers[i] = static_cast<UBO*> (inf.pMappedData);
            
            auto bufInfo = vk::DescriptorBufferInfo(ubos[i], 0, sizeof(UBO));
            
            device->updateDescriptorSets({
                vk::WriteDescriptorSet(descSets[i], 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &bufInfo, nullptr)
            }, {});
            
        }
        
    }
    
}

UBODescriptor::~UBODescriptor() {
    
    device->destroy(descLayout);
    
    device->destroy(descPool);
    
}
