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
    
    void preinit() {
        
        for(std::unique_ptr<System>& sys : systems) {
            sys->preinit();
        }
        
    }
    
    void init() {
        
        for(std::unique_ptr<System>& sys : systems) {
            sys->init();
        }
        
    }
    
    void tick();
    
    void finish() {
        
        executor.wait_for_all();
        
        for(std::unique_ptr<System>& sys : systems) {
            sys->finish();
        }
        
    }

private:
    std::vector<std::unique_ptr<System>> systems;
    
    tf::Executor executor;
    tf::Taskflow taskflow;
    
    entt::registry& reg;
    
};

#endif
