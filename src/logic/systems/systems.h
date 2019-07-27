#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "system.h"

#include "util/profile.h"

#include <vector>
#include <functional>
#include "entt/entt.hpp"

#include <iostream>
#include <chrono>

#include "taskflow/taskflow.hpp"

class Systems {
public:
    Systems(entt::registry& reg);
    
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
    
    void tick(entt::registry&);
    
    void finish(entt::registry& reg) {
        
        executor.wait_for_all();
        
        for(std::unique_ptr<System>& sys : systems) {
            sys->finish(reg);
        }
        
    }

private:
    std::vector<std::unique_ptr<System>> systems;
    
    tf::Executor executor;
    tf::Taskflow taskflow;
    
};

#endif
