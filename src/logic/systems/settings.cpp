#include "logic/systems/system_list.h"

#include <filesystem>
#include <fstream>

#include "logic/components/settings.h"
#include "cereal/archives/json.hpp"

#ifndef DYNAMICAL_CONFIG_FILE
#define DYNAMICAL_CONFIG_FILE "./config.json"
#endif

void synchronize(entt::registry& reg, Settings& settings) {
    if(std::filesystem::exists(DYNAMICAL_CONFIG_FILE)) {
        std::ifstream is(DYNAMICAL_CONFIG_FILE);
        int i = is.get();
        if(i == Settings::magic_number) {
            cereal::JSONInputArchive in(is);
            settings.serialize(in);
            return;
        }
    }

    std::ofstream os(DYNAMICAL_CONFIG_FILE);
    os.put(Settings::magic_number);
    cereal::JSONOutputArchive out(os);
    settings.serialize(out);
}

SettingSys::SettingSys(entt::registry& reg) : System(reg) {
    auto& settings = reg.set<Settings>();
    synchronize(reg, settings);
    settings.argument_override(reg.ctx<Arguments>());
}
