#include "systems.h"

#include "system_list.h"
#include "renderer/renderer.h"
#include "renderer/marching_cubes/terrain.h"
#include "renderer/marching_cubes/marching_cubes.h"
#include "logic/components/settings.h"
#include "client_network.h"
#include "server_network.h"

#define MAKE_SYSTEM(TYPE, NAME) \
systems.push_back(std::make_unique<TYPE>(reg)); \
auto NAME = systems[i].get(); \
i++;

Systems::Systems(entt::registry& reg) : reg(reg) {

    reg.set<tf::Executor>();
    
    int i = 0;
    MAKE_SYSTEM(SettingSys, settings)
    Settings& s = reg.ctx<Settings>();
    
#ifndef NDEBUG
    if(s.server_side) {
        std::cout << "server_side" << std::endl;
    } else if(s.client_side) {
        std::cout << "client_side" << std::endl;
    } else {
        std::cout << "single_player" << std::endl;
    }
#endif
    
    
    if(s.server_side) {
        
        MAKE_SYSTEM(ServerNetworkSys, server)
        
        taskflow.emplace([=]() {server->tick();});
        
    } else if(s.client_side) {
        
        MAKE_SYSTEM(InputSys, input)
        MAKE_SYSTEM(DebugSys, debug)
        MAKE_SYSTEM(UISys, ui)
        MAKE_SYSTEM(CameraSys, camera)
        MAKE_SYSTEM(ChunkManagerSys, chunk_manager)
        MAKE_SYSTEM(ChunkLoaderSys, chunk_loader)
        MAKE_SYSTEM(ChunkGeneratorSys, chunk_generator)
        MAKE_SYSTEM(ChunkSys, chunk)
        MAKE_SYSTEM(Terrain, terrain)
        MAKE_SYSTEM(MarchingCubes, marching_cubes)
        MAKE_SYSTEM(Renderer, renderer)
        
        MAKE_SYSTEM(ClientNetworkSys, client)
        
        tf::Task input_t = taskflow.emplace([=]() {
            input->tick();
#ifndef NDEBUG
            debug->tick();
#endif
            ui->tick();
            camera->tick();
        });
        
        auto chunks_t = taskflow.emplace(
            [=]() {
                chunk_manager->tick();
                chunk_loader->tick();
                chunk_generator->tick();
                chunk->tick();
                terrain->tick();
                marching_cubes->tick();
            }
        );
        
        tf::Task renderer_t = taskflow.emplace([=]() {renderer->tick();});
        
        
        taskflow.emplace([=]() {client->tick();});
        
        renderer_t.gather(input_t);
        
        chunks_t.gather(input_t);
    
    } else {
        
        MAKE_SYSTEM(InputSys, input)
        MAKE_SYSTEM(DebugSys, debug)
        MAKE_SYSTEM(UISys, ui)
        MAKE_SYSTEM(CameraSys, camera)
        MAKE_SYSTEM(ChunkManagerSys, chunk_manager)
        MAKE_SYSTEM(ChunkLoaderSys, chunk_loader)
        MAKE_SYSTEM(ChunkGeneratorSys, chunk_generator)
        MAKE_SYSTEM(ChunkSys, chunk)
        MAKE_SYSTEM(Terrain, terrain)
        MAKE_SYSTEM(MarchingCubes, marching_cubes)
        MAKE_SYSTEM(Renderer, renderer)
        
        tf::Task input_t = taskflow.emplace([=]() {
            input->tick();
#ifndef NDEBUG
            debug->tick();
#endif
            ui->tick();
            camera->tick();
        });
        
        auto chunks_t = taskflow.emplace(
            [=]() {
                chunk_manager->tick();
                chunk_loader->tick();
                chunk_generator->tick();
                chunk->tick();
                terrain->tick();
                marching_cubes->tick();
            }
        );
        
        tf::Task renderer_t = taskflow.emplace([=]() {renderer->tick();});
        
        renderer_t.gather(input_t);
        
        chunks_t.gather(input_t);
        
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
