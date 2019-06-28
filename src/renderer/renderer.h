#ifndef RENDERER_H
#define RENDERER_H

#include "windu.h"
#include "instance.h"
#include "device.h"
#include "swapchain.h"
#include "camera.h"
#include "terrain.h"
#include "marching_cubes/marching_cubes.h"
#include "main_render/main_render.h"

#include "entt/entt.hpp"

class Renderer {
public:
    Renderer();
    void init(entt::registry& reg);
    void render(entt::registry& reg);
    ~Renderer();
    
    void resize();
    
private:
    Windu win;
    Instance instance;
    Device device;
    Swapchain swap;
    Camera camera;
    Terrain terrain;
    MarchingCubes marching_cubes;
    MainRender main_render;
    
    int semindex = 0;
    std::vector<vk::Semaphore> waitsems;
    std::vector<vk::Semaphore> signalsems;
};

#endif
