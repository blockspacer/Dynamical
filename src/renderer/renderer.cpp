#include "renderer.h"

Renderer::Renderer() : win(), instance(win), device(instance), swap(win, instance, device), main_render(instance, device, swap) {
    
    
}

void Renderer::init() {
    
}

Renderer::~Renderer() {
    
}
