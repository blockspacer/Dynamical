#include "system_list.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/norm.hpp"

#include "logic/components/chunkc.h"
#include "util/util.h"

#include "logic/components/chunkdatac.h"
#include "logic/components/chunk_map.h"
#include "logic/components/camerac.h"
#include "renderer/camera.h"
#include "renderer/marching_cubes/chunk.h"
#include "renderer/marching_cubes/marching_cubes.h"
#include "logic/components/renderinfo.h"

#include "FastNoiseSIMD/FastNoiseSIMD/FastNoiseSIMD.h"
#include "taskflow/taskflow.hpp"

//constexpr float render_chunks = render_distance / chunk::base_length + 1;

const static std::unique_ptr<FastNoiseSIMD> myNoise = std::unique_ptr<FastNoiseSIMD>(FastNoiseSIMD::NewFastNoiseSIMD());
constexpr double frequency = 0.0005;
constexpr double amplitude = 500.;
constexpr int octaves = 6;

void ChunkSys::init() {
    
    ChunkMap& map = reg.set<ChunkMap>();
    
    myNoise->SetFractalOctaves(octaves);
    myNoise->SetFrequency(frequency);
    
}


void ChunkSys::tick() {
    
    ChunkMap& map = reg.ctx<ChunkMap>();
    CameraC cam = reg.ctx<CameraC>();
    
    ChunkC chunk;
    const int lod = 5;
    chunk.lod = lod;
    const int mul = chunk.getLOD();
    const int render_chunks = render_distance / (chunk.getSize().x)+1;
    reg.view<ChunkC>().each([&](entt::entity entity, ChunkC& chunk) {
        if(glm::distance2(glm::vec3(chunk.getPosition()), cam.pos) > Util::sq(render_distance + chunk.getSize().x*2)) {
            map.set(entt::null, chunk.pos.x, chunk.pos.y, chunk.pos.z, mul/2);
            if(!reg.has<entt::tag<"destroying"_hs>>(entity)) reg.assign<entt::tag<"destroying"_hs>>(entity);
        }
    });
    
    
    
    auto& cd = reg.ctx<ChunkDataC>();
    auto ri = reg.ctx<RenderInfo>();
    
    typedef int prepare;
    
    for(int x = -render_chunks; x<=render_chunks; x++) {
        for(int z = -render_chunks; z<=render_chunks; z++) {
            for(int y = -render_chunks; y<=render_chunks; y++) {
                chunk.pos = glm::vec3(x + std::round(cam.pos.x/(mul*chunk::base_length)), y + std::round(cam.pos.y/(mul*chunk::base_length)), z + std::round(cam.pos.z/(mul*chunk::base_length))) * (float) mul;
                if(map.get(chunk.pos.x, chunk.pos.y, chunk.pos.z, mul/2) == entt::null && glm::distance2(glm::vec3(chunk.getPosition()), cam.pos) < Util::sq(render_distance + chunk.getSize().x)) {
                    auto entity = reg.create();
                    ChunkC& cc = reg.assign<ChunkC>(entity);
                    cc.pos = chunk.pos;
                    cc.lod = chunk.lod;
                    reg.assign<prepare>(entity, -1);
                    map.set(entity, chunk.pos.x, chunk.pos.y, chunk.pos.z, mul/2);
                    
                }
            }
        }
    }
    
    static std::mutex mutex;
    int i = cd.index[ri.frame_index];
    
    tf::Executor executor;
    tf::Taskflow taskflow;
    
    auto view = reg.view<ChunkC, prepare>();
    taskflow.parallel_for(view.begin(), view.end(), [&] (const entt::entity entity) { 
        
        mutex.lock();
        int si = i;
        mutex.unlock();
        
        if(si >= max_per_frame) return;
        
        auto& chunk = view.get<ChunkC>(entity);
        
        const int mul = chunk.getLOD();
        const auto cubeSize = chunk.getCubeSize();
        float* noise = myNoise->GetSimplexFractalSet(
            chunk::num_cubes.x * chunk.pos.x/mul - chunk::border, chunk::num_cubes.z * chunk.pos.z/mul - chunk::border, chunk::num_cubes.y * chunk.pos.y/mul - chunk::border,
            chunk::num_values.x, chunk::num_values.z, chunk::num_values.y, cubeSize);
    
        int index = 0;
        
        bool sign;
        
        bool empty = true;
        for(int x = 0; x < chunk::num_values.x; x++) {
            for(int z = 0; z < chunk::num_values.z; z++) {
                for(int y = 0; y < chunk::num_values.y; y++) {
                    
                    int rx = chunk::base_size.x * chunk.pos.x + x * cubeSize;
                    int rz = chunk::base_size.z * chunk.pos.z + z * cubeSize;
                    int ry = chunk::base_size.y * chunk.pos.y + y * cubeSize;
                    
                    float value = 70 - ry + amplitude * noise[index];
                    
                    if(x == 0 && y == 0 && z == 0) {
                        sign = std::signbit(value);
                    } else if(sign != std::signbit(value)) {
                        empty = false;
                    }
                    noise[index] = value;
                    index++;
                    
                }
            }
        }
        
        auto& full = view.get<prepare>(entity);
        if(empty) {
            full = -2;
            return;
        }
        
        mutex.lock();
        si = i;
        i++;
        //std::cout << si << std::endl;
        mutex.unlock();
        full = si;
        if(si >= max_per_frame) return;
        
        ChunkData* chunkData = cd.data[ri.frame_index] + si;
        
        memcpy(chunkData, noise, sizeof(ChunkData));
        FastNoiseSIMD::FreeNoiseSet(noise);
        
    });
    
    executor.run(taskflow).wait();
    
    cd.index[ri.frame_index] = std::min(i, max_per_frame);
    
    auto endView = reg.view<prepare>();
    endView.each([&](entt::entity entity, int& si) {
        if(si == -2) {
            reg.remove<prepare>(entity);
        } else if(si >= 0 && si < max_per_frame) {
            reg.assign<entt::tag<"modified"_hs>>(entity);
            ChunkBuild& cb = reg.assign<ChunkBuild>(entity);
            cb.index = si;
            reg.remove<prepare>(entity);
        }
    });
    
}
