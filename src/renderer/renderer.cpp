#include "renderer.h"

#include <iostream>

#include "logic/components/inputc.h"
#include "num_frames.h"
#include "logic/components/renderinfo.h"

Renderer::Renderer(entt::registry& reg) : System(reg), win(), instance(win), device(instance), transfer(device), swap(win, instance, device), camera(swap.extent.width, swap.extent.height), main_render(instance, device, transfer, swap, camera),
waitsems(NUM_FRAMES), signalsems(NUM_FRAMES), computesems(NUM_FRAMES) {
    
    for(int i = 0; i < waitsems.size(); i++) {
        waitsems[i] = device->createSemaphore({});
        signalsems[i] = device->createSemaphore({});
        computesems[i] = device->createSemaphore({});
    }
    
    transfer.flush();
    
}

void Renderer::preinit() {
    
    reg.set<SDL_Window*>(win);
    
    reg.set<Device*>(&device);
    
    auto& ri = reg.set<RenderInfo>();
    ri.frame_index = 0;
    ri.frame_num = 0;
    
}

void Renderer::init() {
    
}

void Renderer::tick() {
    
    auto& ri = reg.ctx<RenderInfo>();
    
    ri.frame_index = (ri.frame_index+1)%NUM_FRAMES;
    ri.frame_num++;
    
    InputC& input = reg.ctx<InputC>();
    if(input.on[Action::RESIZE]) {
        resize();
        input.on.set(Action::RESIZE, false);
    }
    
    try {
        
        transfer.flush();
        
        camera.update(reg);
        
        uint32_t index = swap.acquire(waitsems[ri.frame_index]);
        
        main_render.render(reg, index, {waitsems[ri.frame_index]}, {signalsems[ri.frame_index]});
        
        swap.present(signalsems[ri.frame_index]);
        
    } catch(vk::OutOfDateKHRError&) {
        
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
