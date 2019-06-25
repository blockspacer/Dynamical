#include "systems.h"

#include "system_list.h"

Systems::Systems() : systems() {
    
    systems.reserve(2);
    systems.push_back(std::make_unique<InputSys>());
    systems.push_back(std::make_unique<CameraSys>());
    
}
