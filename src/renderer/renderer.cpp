#include "renderer.h"

#include <iostream>

#include "logic/components/inputc.h"

Renderer::Renderer() : win(), instance(win), device(instance), swap(win, instance, device), camera(swap.extent.width, swap.extent.height), terrain(device), marching_cubes(device, terrain), main_render(instance, device, swap, camera, terrain),
waitsems(swap.NUM_FRAMES), signalsems(swap.NUM_FRAMES) {
    
    for(int i = 0; i < waitsems.size(); i++) {
        waitsems[i] = device->createSemaphore({});
        signalsems[i] = device->createSemaphore({});
    }
    
    marching_cubes.compute(nullptr, nullptr);
    
}

void Renderer::init(entt::registry& reg) {
    
    reg.set<SDL_Window*>(win);
    
}

void Renderer::render(entt::registry& reg) {
    
    InputC& input = reg.ctx<InputC>();
    if(input.on[Action::RESIZE]) {
        resize();
        input.on.set(Action::RESIZE, false);
    }
    
    try {
        
        camera.update(reg);
    
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
        
        camera.setup(swap.extent.width, swap.extent.height);
        
        main_render.rsetup();
    
    }
    
}
