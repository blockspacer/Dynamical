#include "systems.h"

#include "system_list.h"
#include "renderer/renderer.h"
#include "renderer/marching_cubes/terrain.h"
#include "renderer/marching_cubes/marching_cubes.h"

#define MAKE_SYSTEM(TYPE, NAME) \
systems.push_back(std::make_unique<TYPE>(reg)); \
auto NAME = systems[i].get(); \
i++;

Systems::Systems(entt::registry& reg) : reg(reg) {

    tf::Executor& executor = reg.set<tf::Executor>();
    
    int i = 0;
    MAKE_SYSTEM(SettingSys, settings)
    MAKE_SYSTEM(InputSys, input)
    MAKE_SYSTEM(CameraSys, camera)
    MAKE_SYSTEM(ChunkSys, chunk)
    MAKE_SYSTEM(Terrain, terrain)
    MAKE_SYSTEM(MarchingCubes, marching_cubes)
    MAKE_SYSTEM(Renderer, renderer)
    
    tf::Task input_t = taskflow.emplace([=]() {input->tick();});
    tf::Task camera_t = taskflow.emplace([=]() {camera->tick();});
    tf::Task renderer_t = taskflow.emplace([=]() {renderer->tick();});
    
    {
        tf::Task chunk_t = chunk_taskflow.emplace([=]() {chunk->tick();});
        tf::Task terrain_t = chunk_taskflow.emplace([=]() {terrain->tick();});
        tf::Task marching_cubes_t = chunk_taskflow.emplace([=]() {marching_cubes->tick();});
        chunk_t.precede(terrain_t);
        terrain_t.precede(marching_cubes_t);
    }
    
    input_t.precede(camera_t);
    camera_t.precede(renderer_t);
    
}

void Systems::preinit() {
    
    for(std::unique_ptr<System>& sys : systems) {
        sys->preinit();
    }
    
}

void Systems::init() {
    
    for(std::unique_ptr<System>& sys : systems) {
        sys->init();
    }
    
    reg.ctx<tf::Executor>().run_until(chunk_taskflow, [this]() {return !running;});
    
}

void Systems::tick() {
    
    tf::Executor& executor = reg.ctx<tf::Executor>();
    executor.run(taskflow).wait();
}

void Systems::finish() {
        
    tf::Executor& executor = reg.ctx<tf::Executor>();
    running = false;
    executor.wait_for_all();
    
    for(std::unique_ptr<System>& sys : systems) {
        sys->finish();
    }
    
}
