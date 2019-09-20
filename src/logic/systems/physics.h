#ifndef PHYSICS_H
#define PHYSICS_H

#include "system.h"

class PhysicsSys : public System {
public:
    PhysicsSys(entt::registry& reg) : System(reg) {};
    void preinit() override;
    void init() override;
    void tick() override;
    const char* name() override {return "Physics";};
};

#endif
