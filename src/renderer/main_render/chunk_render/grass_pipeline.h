#ifndef GRASS_PIPELINE_H
#define GRASS_PIPELINE_H

#include "vulkan/vulkan.hpp"

#include "renderer/vmapp.h"

#include "glm/glm.hpp"

#include "renderer/num_frames.h"

class Device;
class Transfer;
class Swapchain;
class Renderpass;
class UBODescriptor;

class GrassPipeline {
public:
    GrassPipeline(Device& device, Transfer& transfer, Swapchain& swap, Renderpass& renderpass, UBODescriptor& ubo);
    ~GrassPipeline();
    
    operator vk::Pipeline() { return pipeline; }
    operator vk::PipelineLayout() { return layout; }
    
    vk::DescriptorPool descPool;
    vk::DescriptorSetLayout descLayout;
    vk::DescriptorSet descSet;
    
    vk::Sampler sampler;
    
    VmaImage raycastImage;
    vk::ImageView raycastImageView;
    
    vk::PipelineLayout layout;
    vk::Pipeline pipeline;
    
private:
    Device& device;
    Transfer& transfer;
    Swapchain& swap;
    Renderpass& renderpass;
    
};

#endif