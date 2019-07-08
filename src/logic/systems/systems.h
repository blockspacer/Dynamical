#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "system.h"

#include <vector>
#include <functional>
#include "entt/entt.hpp"

#include <iostream>

class Systems {
public:
    Systems();
    
    void preinit(entt::registry& reg) {
        
        for(std::unique_ptr<System>& sys : systems) {
            sys->preinit(reg);
        }
        
    }
    
    void init(entt::registry& reg) {
        
        for(std::unique_ptr<System>& sys : systems) {
            sys->init(reg);
        }
        
    }
    
    void tick(entt::registry& reg) {
        
        for(std::unique_ptr<System>& sys : systems) {
            sys->tick(reg);
        }
        
    }

private:
    std::vector<std::unique_ptr<System>> systems;
};

#endif
