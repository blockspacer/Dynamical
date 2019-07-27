#include "systems.h"

#include "system_list.h"
#include "renderer/renderer.h"
#include "renderer/marching_cubes/terrain.h"
#include "renderer/marching_cubes/marching_cubes.h"

#define MAKE_SYSTEM(TYPE, NAME) \
systems.push_back(std::make_unique<TYPE>()); \
tf::Task NAME = taskflow.emplace([=]() {systems[i]->tick(*preg);}); \
i++;


constexpr size_t system_count = 6;

Systems::Systems(entt::registry& reg) : systems() {
    
    systems.reserve(system_count);
    entt::registry* preg = &reg;
    int i = 0;
    
    MAKE_SYSTEM(InputSys, input)
    MAKE_SYSTEM(CameraSys, camera)
    MAKE_SYSTEM(ChunkSys, chunk)
    MAKE_SYSTEM(Terrain, terrain)
    MAKE_SYSTEM(MarchingCubes, marching_cubes)
    MAKE_SYSTEM(Renderer, renderer)
    Renderer* rend = static_cast<Renderer*> (systems[i-1].operator->());
    tf::Task renderer_prep = taskflow.emplace([=]() {rend->prepare(*preg);});
    
    input.precede(camera);
    camera.precede(renderer_prep);
    renderer_prep.precede(chunk, renderer);
    
    chunk.precede(terrain);
    terrain.precede(marching_cubes);

}


void Systems::tick(entt::registry&) {
    
    /*
    for(std::unique_ptr<System>& sys : systems) {
        if(systemprofiling) {
            auto before = std::chrono::high_resolution_clock::now();
            sys->tick(reg);
            auto after = std::chrono::high_resolution_clock::now();
            auto milliseconds = std::chrono::duration_cast<std::chrono::nanoseconds> (after - before).count()/1000000.;
            if(milliseconds > 20.) std::cout << sys->name() << " system took " << milliseconds << " milliseconds" << std::endl;
        } else {
            sys->tick(reg);
        }
    }
    */
    
    executor.run(taskflow).wait();
    
}
