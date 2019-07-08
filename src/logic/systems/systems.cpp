#include "systems.h"

#include "system_list.h"
#include "renderer/renderer.h"

Systems::Systems() : systems() {
    
    systems.reserve(4);
    systems.push_back(std::make_unique<InputSys>());
    systems.push_back(std::make_unique<CameraSys>());
    systems.push_back(std::make_unique<ChunkSys>());
    systems.push_back(std::make_unique<Renderer>());
    
}
