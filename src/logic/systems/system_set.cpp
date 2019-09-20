#include "system_set.h"

void SystemSet::add(System* system) {
    systems.push_back(system);
}

void SystemSet::preinit() {
    
    for(System* sys : systems) {
        sys->preinit();
    }
    
}

void SystemSet::init() {
    
    for(System* sys : systems) {
        sys->init();
    }
    
}

void SystemSet::tick() {
    
    for(System* sys : systems) {
        sys->tick();
    }
    
}

void SystemSet::finish() {
    
    for(System* sys : systems) {
        sys->finish();
    }
    
}
