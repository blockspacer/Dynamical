#ifndef BASIC_PIPELINE_H
#define BASIC_PIPELINE_H

#include "vulkan/vulkan.hpp"

#include "renderer/vmapp.h"

class Device;
class Transfer;
class Swapchain;
class Renderpass;

constexpr int num_textures = 2;

class ChunkPipeline {
public:
    ChunkPipeline(Device& device, Transfer& transfer, Swapchain& swap, Renderpass& renderpass);
    ~ChunkPipeline();
    
    operator vk::Pipeline() { return pipeline; }
    operator vk::PipelineLayout() { return layout; }
    
    vk::DescriptorPool descPool;
    vk::DescriptorSetLayout descLayout;
    std::vector<vk::DescriptorSet> descSets;
    
    vk::DescriptorSetLayout materialLayout;
    vk::DescriptorSet materialSet;
    
    VmaImage materialTexture;
    vk::ImageView materialTextureView;
    
    vk::Sampler sampler;
    
    vk::PipelineLayout layout;
    vk::Pipeline pipeline;
    
private:
    Device& device;
    Transfer& transfer;
    Swapchain& swap;
    Renderpass& renderpass;
    
};

#endif
