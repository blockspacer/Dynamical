#ifndef RENDERER_H
#define RENDERER_H

#include "windu.h"
#include "instance.h"
#include "device.h"
#include "swapchain.h"
#include "camera.h"
#include "main_render/main_render.h"

#include "logic/systems/system.h"

#include "entt/entt.hpp"

class Renderer : public System {
public:
    Renderer();
    void preinit(entt::registry& reg) override;
    void init(entt::registry& reg) override;
    void tick(entt::registry& reg) override;
    const char* name() override {return "Renderer";};
    ~Renderer() override;
    
    void resize();
    
private:
    Windu win;
    Instance instance;
    Device device;
    Swapchain swap;
    Camera camera;
    MainRender main_render;
    
    int frame_num = 0;
    int semindex = 0;
    std::vector<vk::Semaphore> waitsems;
    std::vector<vk::Semaphore> signalsems;
    std::vector<vk::Semaphore> computesems;
};

#endif
