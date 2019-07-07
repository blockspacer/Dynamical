#ifndef MAIN_RENDER_H
#define MAIN_RENDER_H

#include "renderpass.h"
#include "basic_pipeline.h"

#include "renderer/vmapp.h"

#include "glm/glm.hpp"

#include "entt/entt.hpp"

class Instance;
class Device;
class Swapchain;
class Camera;
class Terrain;


struct UBO {
    
    glm::mat4 viewproj;
    
};


class MainRender {
public:
    MainRender(Instance& instance, Device& device, Swapchain& swap, Camera& camera, Terrain& terrain);
    void setup();
    void rsetup();
    void render(entt::registry& reg, uint32_t index, std::vector<vk::Semaphore> waits, std::vector<vk::Semaphore> signals);
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
    Camera& camera;
    Terrain& terrain;
    
    vk::CommandPool commandPool;
    std::vector<vk::CommandBuffer> commandBuffers;
    std::vector<vk::Fence> fences;
    
    std::vector<VmaBuffer> ubos;
    std::vector<UBO*> uboPointers;
    
};

#endif
