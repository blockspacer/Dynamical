#ifndef MAIN_RENDER_H
#define MAIN_RENDER_H

#include "renderpass.h"
#include "basic_pipeline.h"

#include "renderer/vmapp.h"

#include "glm/glm.hpp"

#include "entt/entt.hpp"

#include "renderer/num_frames.h"

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
    MainRender(Instance& instance, Device& device, Swapchain& swap, Camera& camera);
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
    
    vk::CommandPool commandPool;
    std::array<vk::CommandBuffer, NUM_FRAMES> commandBuffers;
    std::array<vk::Fence, NUM_FRAMES> fences;
    
    std::array<VmaBuffer, NUM_FRAMES> ubos;
    std::array<UBO*, NUM_FRAMES> uboPointers;
    
};

#endif
