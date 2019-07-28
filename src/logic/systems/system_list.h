#ifndef SYSTEM_LIST_H
#define SYSTEM_LIST_H

#include "entt/entt.hpp"

#include "system.h"

class InputSys : public System {
public:
    InputSys(entt::registry& reg) : System(reg) {};
    void preinit() override;
    void init() override;
    void tick() override;
    const char* name() override {return "Input";};
};

class CameraSys : public System {
public:
    CameraSys(entt::registry& reg) : System(reg) {};
    void init() override;
    void tick() override;
    const char* name() override {return "Camera";};
};

class ChunkSys : public System {
public:
    ChunkSys(entt::registry& reg) : System(reg) {};
    void init() override;
    void tick() override;
    const char* name() override {return "Chunk";};
};

#endif
