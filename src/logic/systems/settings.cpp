#include "logic/systems/system_list.h"

#include <filesystem>
#include <fstream>

#include "logic/components/settings.h"
#include "cereal/archives/json.hpp"

#ifndef DYNAMICAL_CONFIG_FILE
#define DYNAMICAL_CONFIG_FILE "./config.js"
#endif

void saveSettings(Settings&& settings) {
    std::ofstream os(DYNAMICAL_CONFIG_FILE);
    cereal::JSONOutputArchive out(os);
    out(settings);
}

void SettingSys::preinit() {
    auto& settings = reg.set<Settings>();

    if(std::filesystem::exists(DYNAMICAL_CONFIG_FILE)) {
        std::ifstream is(DYNAMICAL_CONFIG_FILE);
        cereal::JSONInputArchive in(is);
        in(settings);
    } else {
        saveSettings(Settings());
    }
}

