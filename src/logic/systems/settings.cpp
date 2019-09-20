#include "settings.h"

#include <filesystem>
#include <fstream>

#include "logic/components/settingsc.h"
#include "cereal/archives/json.hpp"

#ifndef DYNAMICAL_CONFIG_FILE
#define DYNAMICAL_CONFIG_FILE "./config.json"
#endif

void synchronize(entt::registry& reg, SettingsC& settings) {
    if(std::filesystem::exists(DYNAMICAL_CONFIG_FILE)) {
        std::ifstream is(DYNAMICAL_CONFIG_FILE);
        int i = is.get();
        if(i == SettingsC::magic_number) {
            cereal::JSONInputArchive in(is);
            settings.serialize(in);
            return;
        }
    }

    std::ofstream os(DYNAMICAL_CONFIG_FILE);
    os.put(SettingsC::magic_number);
    cereal::JSONOutputArchive out(os);
    settings.serialize(out);
}

SettingSys::SettingSys(entt::registry& reg, int argc, char** argv) : System(reg) {
    auto& settings = reg.set<SettingsC>();
    synchronize(reg, settings);
    settings.argument_override(argc, argv);
}
