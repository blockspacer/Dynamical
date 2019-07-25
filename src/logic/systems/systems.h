#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "system.h"

#include "util/profile.h"

#include <vector>
#include <functional>
#include "entt/entt.hpp"

#include <iostream>
#include <chrono>

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
            if(systemprofiling) {
                auto before = std::chrono::high_resolution_clock::now();
                sys->tick(reg);
                auto after = std::chrono::high_resolution_clock::now();
                auto milliseconds = std::chrono::duration_cast<std::chrono::nanoseconds> (after - before).count()/1000000.;
                if(milliseconds > 20.) std::cout << sys->name() << " system took " << milliseconds << " milliseconds" << std::endl;
            } else {
                sys->tick(reg);
            }
        }
        
    }

private:
    std::vector<std::unique_ptr<System>> systems;
};

#endif
