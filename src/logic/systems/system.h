#ifndef SYSTEM_H
#define SYSTEM_H

#include "entt/entt.hpp"

#include <iostream>

class System {
public:
    virtual void preinit(entt::registry& reg) {}
    virtual void init(entt::registry& reg) = 0;
    virtual void tick(entt::registry& reg) = 0;
    virtual ~System() {}
};

#endif
