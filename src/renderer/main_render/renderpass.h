#ifndef RENDERPASS_H
#define RENDERPASS_H

#include "vulkan/vulkan.hpp"
#include "renderer/vmapp.h"

class Device;
class Swapchain;

class Renderpass {
public:
    Renderpass(Device& device, Swapchain& swap);
    void setup();
    void cleanup();
    ~Renderpass();
    
    operator vk::RenderPass() { return renderpass; }
    operator VkRenderPass() { return static_cast<VkRenderPass>(renderpass); }
    
    vk::Format depthFormat;
    std::vector<VmaImage> depthImages;
    std::vector<vk::ImageView> depthViews;
    std::vector<vk::Framebuffer> framebuffers;
    vk::RenderPass renderpass;
    
private:
    Device& device;
    Swapchain& swap;
    
};

#endif
