#ifndef MAIN_RENDER_H
#define MAIN_RENDER_H

#include "renderpass.h"

class Instance;
class Device;
class Swapchain;

class MainRender {
public:
    MainRender(Instance& instance, Device& device, Swapchain& swap);
    ~MainRender();
    void render();
    
private:
    Renderpass renderpass;
    Instance& instance;
    Device& device;
    Swapchain& swap;
    
};

#endif
