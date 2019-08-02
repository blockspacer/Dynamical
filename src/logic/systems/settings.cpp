#include "logic/systems/system_list.h"

#include <filesystem>
#include <fstream>

#include "logic/components/settings.h"
#include "cereal/archives/json.hpp"

#ifndef DYNAMICAL_CONFIG_FILE
#define DYNAMICAL_CONFIG_FILE "./config.json"
#endif

void SettingSys::preinit() {
    auto& settings = reg.set<Settings>();
    
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

