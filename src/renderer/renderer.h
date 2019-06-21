#ifndef RENDERER_H
#define RENDERER_H

#include "windu.h"
#include "instance.h"
#include "device.h"
#include "swapchain.h"
#include "main_render/main_render.h"
#include "marching_cubes/marching_cubes.h"

class Renderer {
public:
    Renderer();
    void init();
    void render();
    ~Renderer();
    
    void resize();
    
private:
    Windu win;
    Instance instance;
    Device device;
    Swapchain swap;
    MainRender main_render;
    MarchingCubes marching_cubes;
    
    int semindex = 0;
    std::vector<vk::Semaphore> waitsems;
    std::vector<vk::Semaphore> signalsems;
};

#endif
