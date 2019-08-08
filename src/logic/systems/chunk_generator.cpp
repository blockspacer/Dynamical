#include "system_list.h"

#include "FastNoiseSIMD/FastNoiseSIMD/FastNoiseSIMD.h"
#include "taskflow/taskflow.hpp"

#include "logic/components/chunk_map.h"
#include "logic/components/chunkc.h"
#include "logic/components/global_chunk_data.h"
#include "logic/components/sparse_chunk.h"

const static std::unique_ptr<FastNoiseSIMD> myNoise = std::unique_ptr<FastNoiseSIMD>(FastNoiseSIMD::NewFastNoiseSIMD());
constexpr double frequency = 0.0001;
constexpr double amplitude = 4000.;
constexpr int octaves = 6;

void ChunkGeneratorSys::init() {
    
    myNoise->SetFractalOctaves(octaves);
    myNoise->SetFrequency(frequency);
    
}

void ChunkGeneratorSys::tick() {
    
    tf::Executor& executor = reg.ctx<tf::Executor>();
    tf::Taskflow taskflow;
    
    reg.view<entt::tag<"loading"_hs>>().each([this](const entt::entity entity, auto) {
        reg.assign<GlobalChunkData>(entity);
        reg.assign<GlobalChunkEmpty>(entity, 0.f);
    });
    
    auto view = reg.view<GlobalChunkC, GlobalChunkData, GlobalChunkEmpty, entt::tag<"loading"_hs>>();
    taskflow.parallel_for(view.begin(), view.end(), [view](const entt::entity entity) { 
        
        auto& chunk = view.get<GlobalChunkC>(entity);
        auto& chunk_data = view.get<GlobalChunkData>(entity);
        auto& chunk_mean = view.get<GlobalChunkEmpty>(entity).mean;
        chunk_mean = 0;
        
        const int mul = chunk.getLOD();
        const auto cubeSize = chunk.getCubeSize();
        myNoise->FillSimplexFractalSet(chunk_data.data.data(),
            chunk::num_cubes.x * chunk.pos.x/mul - chunk::border, chunk::num_cubes.z * chunk.pos.z/mul - chunk::border, chunk::num_cubes.y * chunk.pos.y/mul - chunk::border,
            chunk::max_num_values.x, chunk::max_num_values.z, chunk::max_num_values.y, cubeSize);
        
        int index = 0;
        
        bool sign;
        
        bool empty = true;
        
        float sum = 0;
        for(int x = 0; x < chunk::max_num_values.x; x++) {
            for(int z = 0; z < chunk::max_num_values.z; z++) {
                for(int y = 0; y < chunk::max_num_values.y; y++) {
                    
                    int rx = chunk::base_size.x * chunk.pos.x + x * cubeSize;
                    int rz = chunk::base_size.z * chunk.pos.z + z * cubeSize;
                    int ry = chunk::base_size.y * chunk.pos.y + y * cubeSize;
                    
                    float value = 70 - ry + amplitude * chunk_data.data[index];
                    
                    if(x == 0 && y == 0 && z == 0) {
                        sign = std::signbit(value);
                    } else if(sign != std::signbit(value)) {
                        empty = false;
                    }
                    sum += value;
                    chunk_data.data[index] = value;
                    index++;
                    
                }
            }
        }
        
        if(empty) {
            chunk_mean = sum/index;
        } else {
            
            
            SparseChunk* sc = new SparseChunk();
            sc->set(chunk_data);
            
            int index = 0;
            for(int x = 0; x < chunk::max_num_values.x; x++) {
                for(int z = 0; z < chunk::max_num_values.z; z++) {
                    for(int y = 0; y < chunk::max_num_values.y; y++) {
                        chunk_data.data[index++] = 5-y+3*std::sin(x/10.);
                    }
                }
            }
            
            sc->get(chunk_data);
            delete sc;
            
        }
        
    });
    
    executor.run(taskflow).wait();
    taskflow.clear();
    
    auto endView = reg.view<GlobalChunkEmpty, entt::tag<"loading"_hs>>();
    for(auto entity : endView) {
        if(endView.get<GlobalChunkEmpty>(entity).mean != 0) {
            reg.remove<GlobalChunkData>(entity);
        } else {
            reg.remove<GlobalChunkEmpty>(entity);
        }
        reg.remove<entt::tag<"loading"_hs>>(entity);
        reg.assign<entt::tag<"loaded"_hs>>(entity);
    }
    
}
