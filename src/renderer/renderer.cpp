#include "renderer.h"

#include <iostream>

Renderer::Renderer() : win(), instance(win), device(instance), swap(win, instance, device), main_render(instance, device, swap), marching_cubes(device),
waitsems(swap.NUM_FRAMES), signalsems(swap.NUM_FRAMES) {
    
    for(int i = 0; i < waitsems.size(); i++) {
        waitsems[i] = device->createSemaphore({});
        signalsems[i] = device->createSemaphore({});
    }
    
}

void Renderer::init() {
    
}

void Renderer::render() {
    
    try {
    
        uint32_t index = swap.acquire(waitsems[semindex]);
        
        main_render.render(index, waitsems[semindex], signalsems[semindex]);
        
        swap.present(signalsems[semindex]);
        
        semindex = (semindex+1)%swap.NUM_FRAMES;
        
    } catch(vk::OutOfDateKHRError) {
        
        resize();
        
    }
    
}

Renderer::~Renderer() {
    
    device->waitIdle();
    
    for(int i = 0; i < waitsems.size(); i++) {
        device->destroy(waitsems[i]);
        device->destroy(signalsems[i]);
    }
    
}

void Renderer::resize() {
    
    if(win.resize()) {
        
        std::cout << "resize" << std::endl;
        
        device->waitIdle();
        
        main_render.rcleanup();
        
        swap.cleanup();
        
        swap.setup();
        
        main_render.rsetup();
    
    }
    
}
