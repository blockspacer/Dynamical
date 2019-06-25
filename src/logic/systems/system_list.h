#ifndef SYSTEM_LIST_H
#define SYSTEM_LIST_H

#include "entt/entt.hpp"

#include "system.h"

class InputSys : public System {
public:
    virtual void init(entt::registry& reg) override;
    virtual void tick(entt::registry& reg) override;
};

class CameraSys : public System {
public:
    virtual void init(entt::registry& reg) override;
    virtual void tick(entt::registry& reg) override;
};

#endif
