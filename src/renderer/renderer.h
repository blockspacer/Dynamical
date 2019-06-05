#ifndef RENDERER_H
#define RENDERER_H

#include "windu.h"
#include "instance.h"
#include "device.h"
#include "swapchain.h"
#include "main_render/main_render.h"

class Renderer {
public:
    Renderer();
    void init();
    ~Renderer();
private:
    Windu win;
    Instance instance;
    Device device;
    Swapchain swap;
    
    MainRender main_render;
};

#endif
