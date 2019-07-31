#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "system.h"

#include "util/profile.h"

#include <vector>
#include <functional>
#include "entt/entt.hpp"

#include <iostream>
#include <chrono>
#include <mutex>

#include "taskflow/taskflow.hpp"

class Systems {
public:
    Systems(entt::registry& reg);
    
    void preinit();
    
    void init();
    
    void tick();
    
    void finish();
    
    bool running = true;

private:
    std::vector<std::unique_ptr<System>> systems;
    
    tf::Taskflow taskflow;
    tf::Taskflow chunk_taskflow;
    
    entt::registry& reg;
    
};

#endif
