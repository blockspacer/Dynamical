#ifndef BASIC_PIPELINE_H
#define BASIC_PIPELINE_H

#include "vulkan/vulkan.hpp"

class Device;
class Swapchain;
class Renderpass;

class BasicPipeline {
public:
    BasicPipeline(Device& device, Swapchain& swap, Renderpass& renderpass);
    ~BasicPipeline();
    
    operator vk::Pipeline() { return pipeline; }
    operator vk::PipelineLayout() { return layout; }
    
    vk::DescriptorPool descPool;
    vk::DescriptorSetLayout descLayout;
    std::vector<vk::DescriptorSet> descSets;
    
    vk::PipelineLayout layout;
    vk::Pipeline pipeline;
    
private:
    Device& device;
    Swapchain& swap;
    Renderpass& renderpass;
    
};

#endif
