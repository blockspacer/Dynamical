#ifndef SETTINGSC_H
#define SETTINGSC_H

#include <string>

#include "cereal/types/string.hpp"

#include <sstream>

class SettingsC {
public:
    const static char magic_number = 2;
    int window_width = 0;
    int window_height = 0;
    bool fullscreen  = true;
    int fps_max = 60;
    std::string username = "John Doe";
    
    bool server_side = false;
    bool client_side = false;

    template <class Archive>
    void serialize( Archive & ar ) {
        ar(
          CEREAL_NVP(window_width),
          CEREAL_NVP(window_height),
          CEREAL_NVP(fullscreen),
          CEREAL_NVP(fps_max),
          CEREAL_NVP(username),
          CEREAL_NVP(server_side),
          CEREAL_NVP(client_side)
        );
    }
    
    void argument_override(int argc, char** argv) {
        
#define ARGUMENT(T, value) \
if(key == #T) T = value;
        
        for(int i  = 0; i<argc; i++) {
            std::string arg = argv[i];
            std::stringstream ss(arg);
            std::string key;
            std::getline(ss, key, '=');
            std::string value;
            std::getline(ss, value);
            ARGUMENT(window_width, std::atoi(value.c_str()))
            ARGUMENT(window_height, std::atoi(value.c_str()))
            ARGUMENT(fullscreen, value == "true")
            ARGUMENT(fps_max, std::atoi(value.c_str()))
            ARGUMENT(username, value)
            ARGUMENT(server_side, value == "true")
            ARGUMENT(client_side, value == "true")
        }
        
#undef ARGUMENT
        
    }
};


#endif //SETTINGS_H
