#ifndef SETTINGS_H
#define SETTINGS_H

#include "system.h"

class SettingSys : public System {
public:
    SettingSys(entt::registry& reg, int argc, char** argv);
    void preinit() override {};
    void init() override {};
    void tick() override {};
    const char* name() override {return "Setting";};
};

#endif
