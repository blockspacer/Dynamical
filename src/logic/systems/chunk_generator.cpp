#include "system_list.h"

#include "FastNoiseSIMD/FastNoiseSIMD/FastNoiseSIMD.h"
#include "taskflow/taskflow.hpp"

#include "logic/components/chunk_map.h"
#include "logic/components/chunkc.h"
#include "logic/components/global_chunk_data.h"
#include "logic/components/sparse_chunk.h"

const static std::unique_ptr<FastNoiseSIMD> myNoise = std::unique_ptr<FastNoiseSIMD>(FastNoiseSIMD::NewFastNoiseSIMD());
const static std::unique_ptr<FastNoiseSIMD> cavNoise = std::unique_ptr<FastNoiseSIMD>(FastNoiseSIMD::NewFastNoiseSIMD(2010));
constexpr double frequency = 0.001;
constexpr double amplitude = 400.;
constexpr int octaves = 5;

void ChunkGeneratorSys::init() {
    
    myNoise->SetFractalOctaves(octaves);
    myNoise->SetFrequency(frequency);
    cavNoise->SetFractalOctaves(octaves-1);
    cavNoise->SetFrequency(frequency);
    
}

void ChunkGeneratorSys::tick() {
    
    tf::Executor& executor = reg.ctx<tf::Executor>();
    tf::Taskflow taskflow;
    
    reg.view<entt::tag<"loading"_hs>>().each([this](const entt::entity entity, auto) {
        reg.assign<SparseChunk>(entity);
        reg.assign<GlobalChunkEmpty>(entity, 0.f);
    });
    
    auto view = reg.view<GlobalChunkC, SparseChunk, GlobalChunkEmpty, entt::tag<"loading"_hs>>();
    taskflow.parallel_for(view.begin(), view.end(), [view](const entt::entity entity) { 
        
        auto& chunk = view.get<GlobalChunkC>(entity);
        auto& chunk_mean = view.get<GlobalChunkEmpty>(entity).mean;
        chunk_mean = 0;
        
        GlobalChunkData chunk_data;
        
        const int mul = chunk.getLOD();
        const auto cubeSize = chunk.getCubeSize();
        
        myNoise->FillSimplexFractalSet(chunk_data.data.data(),
            chunk::num_cubes.x * chunk.pos.x - chunk::border * chunk::max_mul,
            chunk::num_cubes.z * chunk.pos.z - chunk::border * chunk::max_mul,
            chunk::num_cubes.y * chunk.pos.y - chunk::border * chunk::max_mul,
            chunk::max_num_values.x, chunk::max_num_values.z, chunk::max_num_values.y, chunk::base_cube_size);
        
        float* cav = cavNoise->GetSimplexFractalSet(
            chunk::num_cubes.x * chunk.pos.x - chunk::border * chunk::max_mul,
            chunk::num_cubes.z * chunk.pos.z - chunk::border * chunk::max_mul,
            chunk::num_cubes.y * chunk.pos.y - chunk::border * chunk::max_mul,
            chunk::max_num_values.x, chunk::max_num_values.z, chunk::max_num_values.y, chunk::base_cube_size);
        
        
        int index = 0;
        
        bool sign;
        
        bool empty = true;
        
        float sum = 0;
        for(int x = 0; x < chunk::max_num_values.x; x++) {
            for(int z = 0; z < chunk::max_num_values.z; z++) {
                for(int y = 0; y < chunk::max_num_values.y; y++) {
                    
                    int rx = chunk::base_size.x * chunk.pos.x + x * chunk::base_cube_size;
                    int rz = chunk::base_size.z * chunk.pos.z + z * chunk::base_cube_size;
                    int ry = chunk::base_size.y * chunk.pos.y + y * chunk::base_cube_size;
                    
                    float value = std::min(70 - ry + amplitude * chunk_data.data[index], 70. - Util::s_sq(150.*cav[index]-50.));
                    
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
        
        FastNoiseSIMD::FreeNoiseSet(cav);
        
        if(empty) {
            chunk_mean = sum/index;
        } else {
            auto& sparse_chunk = view.get<SparseChunk>(entity);
            sparse_chunk.set(chunk_data);
        }
        
    });
    
    executor.run(taskflow).wait();
    
    auto endView = reg.view<GlobalChunkEmpty, entt::tag<"loading"_hs>>();
    for(auto entity : endView) {
        if(endView.get<GlobalChunkEmpty>(entity).mean != 0) {
            reg.remove<SparseChunk>(entity);
        } else {
            reg.remove<GlobalChunkEmpty>(entity);
        }
        reg.remove<entt::tag<"loading"_hs>>(entity);
        reg.assign<entt::tag<"loaded"_hs>>(entity);
    }
    
}
