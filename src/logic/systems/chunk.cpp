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

constexpr float render_chunks = render_distance / chunk::base_length + 1;

const static std::unique_ptr<FastNoiseSIMD> myNoise = std::unique_ptr<FastNoiseSIMD>(FastNoiseSIMD::NewFastNoiseSIMD());
constexpr double frequency = 0.03;
constexpr double amplitude = 40.;
constexpr int octaves = 3;

entt::entity make_chunk(entt::registry& reg, int chunk_x, int chunk_y, int chunk_z) {
    
    auto& cd = reg.ctx<ChunkDataC>();
    auto ri = reg.ctx<RenderInfo>();
    if(cd.index[ri.frame_index] >= max_per_frame) return entt::null;
    
    auto entity = reg.create();
    ChunkC& chunk = reg.assign<ChunkC>(entity);
    chunk.pos = glm::vec3(chunk_x, chunk_y, chunk_z);
    chunk.lod = 0;
    
    ChunkData& chunkData = *(cd.data[ri.frame_index] + cd.index[ri.frame_index]);
    
    float* noise = myNoise->GetSimplexFractalSet(
        chunk::num_cubes.x * chunk_x, chunk::num_cubes.z * chunk_z, chunk::num_cubes.y * chunk_y,
        chunk::num_values.x, chunk::num_values.z, chunk::num_values.y);
    
    
    int index = 0;
    
    bool sign;
    bool empty = true;
    for(int x = 0; x < chunk::num_values.x; x++) {
        for(int z = 0; z < chunk::num_values.z; z++) {
            for(int y = 0; y < chunk::num_values.y; y++) {
                
                int rx = chunk::base_size.x * chunk_x + x * chunk::base_cube_size;
                int rz = chunk::base_size.z * chunk_z + z * chunk::base_cube_size;
                int ry = chunk::base_size.y * chunk_y + y * chunk::base_cube_size;
                
                float value = 70. - ry + amplitude * noise[index++];
                
                if(x == 0 && y == 0 && z == 0) {
                    sign = std::signbit(value);
                } else if(sign != std::signbit(value)) {
                    empty = false;
                }
                chunkData.values[x * chunk::num_values.z * chunk::num_values.y + z * chunk::num_values.y + y] = value;
                
            }
        }
    }
    
    FastNoiseSIMD::FreeNoiseSet(noise);
    
    if(empty == true) {
        return entity;
    }
    
    reg.assign<entt::tag<"modified"_hs>>(entity);
    ChunkBuild& cb = reg.assign<ChunkBuild>(entity);
    cb.index = cd.index[ri.frame_index];
    cd.index[ri.frame_index]++;
    
    return entity;
    
}

void ChunkSys::init(entt::registry& reg) {
    
    ChunkMap& map = reg.set<ChunkMap>();
    
    myNoise->SetFractalOctaves(octaves);
    myNoise->SetFrequency(frequency);
    
}


void ChunkSys::tick(entt::registry& reg) {
    
    ChunkMap& map = reg.ctx<ChunkMap>();
    CameraC& cam = reg.ctx<CameraC>();
    
    reg.view<ChunkC>().each([&](entt::entity entity, ChunkC& chunk) {
        if(glm::distance2(glm::vec3(chunk.getPosition()), cam.pos) > Util::c_sq(render_distance + chunk::base_length*2)) {
            map.remove(chunk.pos.x, chunk.pos.y, chunk.pos.z);
            if(!reg.has<entt::tag<"destroying"_hs>>(entity)) reg.assign<entt::tag<"destroying"_hs>>(entity);
        }
    });
    
    
    for(int x = -render_chunks; x<=render_chunks; x++) {
        for(int z = -render_chunks; z<=render_chunks; z++) {
            for(int y = 0; y < map_chunk_height; y++) {
                ChunkC chunk;
                chunk.lod = 0;
                chunk.pos = glm::vec3(x + std::round(cam.pos.x/chunk::base_length), y, z + std::round(cam.pos.z/chunk::base_length));
                
                if(glm::distance2(glm::vec3(chunk.getPosition()), cam.pos) < Util::c_sq(render_distance + chunk::base_length) && 
                    map.get(chunk.pos.x, chunk.pos.y, chunk.pos.z) == entt::null) {
                    
                    auto entity = make_chunk(reg, chunk.pos.x, chunk.pos.y, chunk.pos.z);
                    if(entity == entt::null) return;
                    map.set(chunk.pos.x, chunk.pos.y, chunk.pos.z, entity);
                }
            }
        }
    }
    
}
