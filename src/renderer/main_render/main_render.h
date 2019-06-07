#ifndef MAIN_RENDER_H
#define MAIN_RENDER_H

#include "renderpass.h"
#include "basic_pipeline.h"

class Instance;
class Device;
class Swapchain;

class MainRender {
public:
    MainRender(Instance& instance, Device& device, Swapchain& swap);
    void render(uint32_t index, vk::Semaphore wait, vk::Semaphore signal);
    ~MainRender();
    
private:
    Renderpass renderpass;
    BasicPipeline pipeline;
    Instance& instance;
    Device& device;
    Swapchain& swap;
    
    vk::CommandPool commandPool;
    std::vector<vk::CommandBuffer> commandBuffers;
    std::vector<vk::Fence> fences;
    
};

#endif
