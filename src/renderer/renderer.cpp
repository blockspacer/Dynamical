#include "renderer.h"

#include <iostream>

#include "logic/components/inputc.h"
#include "num_frames.h"
#include "logic/components/renderinfo.h"

Renderer::Renderer() : win(), instance(win), device(instance), swap(win, instance, device), camera(swap.extent.width, swap.extent.height), terrain(device), marching_cubes(device, terrain), main_render(instance, device, swap, camera, terrain),
waitsems(NUM_FRAMES), signalsems(NUM_FRAMES), computesems(NUM_FRAMES) {
    
    for(int i = 0; i < waitsems.size(); i++) {
        waitsems[i] = device->createSemaphore({});
        signalsems[i] = device->createSemaphore({});
        computesems[i] = device->createSemaphore({});
    }
    
}

void Renderer::preinit(entt::registry& reg) {
    
    reg.set<SDL_Window*>(win);
    
    auto& ri = reg.set<RenderInfo>();
    ri.frame_index = 0;
    ri.frame_num = 0;
    
    terrain.preinit(reg);
    
}

void Renderer::init(entt::registry& reg) {
    
    terrain.init(reg);
    
}

void Renderer::tick(entt::registry& reg) {
    
    auto& ri = reg.ctx<RenderInfo>();
    
    terrain.tick(reg);
    
    InputC& input = reg.ctx<InputC>();
    if(input.on[Action::RESIZE]) {
        resize();
        input.on.set(Action::RESIZE, false);
    }
    
    try {
        
        marching_cubes.compute(reg, ri.frame_index, {}, {});
        
        camera.update(reg);
        
        uint32_t index = swap.acquire(waitsems[ri.frame_index]);
        
        main_render.render(reg, index, {waitsems[ri.frame_index]}, {signalsems[ri.frame_index]});
        
        swap.present(signalsems[ri.frame_index]);
        
        ri.frame_index = (ri.frame_index+1)%NUM_FRAMES;
        ri.frame_num++;
        
    } catch(vk::OutOfDateKHRError) {
        
        resize();
        
    }
    
}

Renderer::~Renderer() {
    
    device->waitIdle();
    
    for(int i = 0; i < waitsems.size(); i++) {
        device->destroy(waitsems[i]);
        device->destroy(signalsems[i]);
        device->destroy(computesems[i]);
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
