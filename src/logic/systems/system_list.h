#ifndef SYSTEM_LIST_H
#define SYSTEM_LIST_H

#include "entt/entt.hpp"

#include "system.h"

class InputSys : public System {
public:
    void preinit(entt::registry& reg) override;
    void init(entt::registry& reg) override;
    void tick(entt::registry& reg) override;
    const char* name() override {return "Input";};
};

class CameraSys : public System {
public:
    void init(entt::registry& reg) override;
    void tick(entt::registry& reg) override;
    const char* name() override {return "Camera";};
};

class ChunkSys : public System {
public:
    void init(entt::registry& reg) override;
    void tick(entt::registry& reg) override;
    const char* name() override {return "Chunk";};
};

#endif
