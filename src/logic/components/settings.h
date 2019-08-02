#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

#include "cereal/types/string.hpp"

class Settings {
public:

    int window_width = 0;
    int window_height = 0;
    bool fullscreen  = true;
    int fps_max = 60;
    std::string username = "John Doe";

    template <class Archive>
    void serialize( Archive & ar ) {
        ar(
          CEREAL_NVP(window_width),
          CEREAL_NVP(window_height),
          CEREAL_NVP(fullscreen),
          CEREAL_NVP(fps_max),
          CEREAL_NVP(username)
        );
    }
};


#endif //SETTINGS_H