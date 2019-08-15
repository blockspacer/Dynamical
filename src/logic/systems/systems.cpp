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

    reg.set<tf::Executor>();
    
    int i = 0;
    MAKE_SYSTEM(SettingSys, settings)
    MAKE_SYSTEM(InputSys, input)
    MAKE_SYSTEM(CameraSys, camera)
    MAKE_SYSTEM(ChunkManagerSys, chunk_manager)
    MAKE_SYSTEM(ChunkLoaderSys, chunk_loader)
    MAKE_SYSTEM(ChunkGeneratorSys, chunk_generator)
    MAKE_SYSTEM(ChunkSys, chunk)
    MAKE_SYSTEM(Terrain, terrain)
    MAKE_SYSTEM(MarchingCubes, marching_cubes)
    MAKE_SYSTEM(Renderer, renderer)
    
    { // Main taskflow
        tf::Task input_t = taskflow.emplace([=]() {input->tick();});
        tf::Task camera_t = taskflow.emplace([=]() {camera->tick();});
        
        auto [chunk_manager_t, chunk_loader_t, chunk_generator_t, chunk_t, terrain_t, marching_cubes_t] = taskflow.emplace(
            [=]() {chunk_manager->tick();},
            [=]() {chunk_loader->tick();},
            [=]() {chunk_generator->tick();},
            [=]() {chunk->tick();},
            [=]() {terrain->tick();},
            [=]() {marching_cubes->tick();}
        );
        
        tf::Task renderer_t = taskflow.emplace([=]() {renderer->tick();});
        
        input_t.precede(camera_t);
        
        renderer_t.gather(camera_t);
        
        chunk_manager_t.gather(camera_t);
        chunk_manager_t.precede(chunk_loader_t);
        chunk_loader_t.precede(chunk_generator_t);
        chunk_generator_t.precede(chunk_t);
        chunk_t.precede(terrain_t);
        terrain_t.precede(marching_cubes_t);
    }
    
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
    
}

void Systems::tick() {
    
    tf::Executor& executor = reg.ctx<tf::Executor>();
    executor.run(taskflow).wait();
}

void Systems::finish() {
        
    tf::Executor& executor = reg.ctx<tf::Executor>();
    running = false;
    executor.wait_for_all();
    taskflow.clear();
    
    for(std::unique_ptr<System>& sys : systems) {
        sys->finish();
    }
    
}
