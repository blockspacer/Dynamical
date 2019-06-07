#include "renderer.h"

Renderer::Renderer() : win(), instance(win), device(instance), swap(win, instance, device), main_render(instance, device, swap),
waitsems(swap.NUM_FRAMES), signalsems(swap.NUM_FRAMES) {
    
    for(int i = 0; i < waitsems.size(); i++) {
        waitsems[i] = device->createSemaphore({});
        signalsems[i] = device->createSemaphore({});
    }
    
}

void Renderer::init() {
    
}

void Renderer::render() {
    
    uint32_t index = swap.acquire(waitsems[semindex]);
    
    main_render.render(index, waitsems[semindex], signalsems[semindex]);
    
    swap.present(signalsems[semindex]);
    
    semindex = (semindex+1)%swap.NUM_FRAMES;
    
}

void Renderer::quit() {
    
    device->waitIdle();
    
}

Renderer::~Renderer() {
    
    for(int i = 0; i < waitsems.size(); i++) {
        device->destroy(waitsems[i]);
        device->destroy(signalsems[i]);
    }
    
}
