#ifndef MAIN_RENDER_H
#define MAIN_RENDER_H

#include "renderpass.h"
#include "basic_pipeline.h"

class Instance;
class Device;
class Swapchain;
class Terrain;

class MainRender {
public:
    MainRender(Instance& instance, Device& device, Swapchain& swap, Terrain& terrain);
    void setup();
    void rsetup();
    void render(uint32_t index, vk::Semaphore wait, vk::Semaphore signal);
    void cleanup();
    void rcleanup();
    void reset();
    ~MainRender();
    
private:
    Renderpass renderpass;
    BasicPipeline pipeline;
    Instance& instance;
    Device& device;
    Swapchain& swap;
    Terrain& terrain;
    
    vk::CommandPool commandPool;
    std::vector<vk::CommandBuffer> commandBuffers;
    std::vector<vk::Fence> fences;
    
};

#endif
