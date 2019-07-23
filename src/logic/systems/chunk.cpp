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

constexpr float render_chunks = render_distance / chunk::base_length + 1;

entt::entity make_chunk(entt::registry& reg, int chunk_x, int chunk_z) {
    
    auto& cd = reg.ctx<ChunkDataC>();
    if(cd.index >= max_per_frame) return entt::null;
    
    auto entity = reg.create();
    ChunkC& chunk = reg.assign<ChunkC>(entity);
    chunk.pos = glm::vec3(chunk_x, 0, chunk_z);
    chunk.lod = 0;
    
    reg.assign<entt::tag<"modified"_hs>>(entity);
    ChunkBuild& cb = reg.assign<ChunkBuild>(entity);
    cb.index = cd.index;
    ChunkData& chunkData = *(cd.data + cd.index);
    cd.index++;
    
    for(int x = 0; x < chunk::num_values.x; x++) {
        for(int z = 0; z < chunk::num_values.z; z++) {
            for(int y = 0; y < chunk::num_values.y; y++) {
                
                int rx = chunk::base_size.x * chunk_x + x * chunk::base_cube_size;
                int rz = chunk::base_size.z * chunk_z + z * chunk::base_cube_size;
                int ry = y * chunk::base_cube_size;
                
                chunkData.values[x * chunk::num_values.z * chunk::num_values.y + z * chunk::num_values.y + y]
                    = 10. - ry + 3.*std::sin((rx + rz) / 20.) + 5.*std::cos((rx - rz)/20.);
                
            }
        }
    }
    
    return entity;
    
}

void ChunkSys::init(entt::registry& reg) {
    
    ChunkMap& map = reg.set<ChunkMap>();
    
}


void ChunkSys::tick(entt::registry& reg) {
    
    ChunkMap& map = reg.ctx<ChunkMap>();
    CameraC& cam = reg.ctx<CameraC>();
    
    
    reg.view<ChunkC>().each([&](entt::entity entity, ChunkC& chunk) {
        if(glm::distance2(glm::vec3(chunk.getPosition()), cam.pos) > Util::c_sq(render_distance + chunk::base_length*2)) {
            map.remove(chunk.pos.x, chunk.pos.z);
            if(!reg.has<entt::tag<"destroying"_hs>>(entity)) reg.assign<entt::tag<"destroying"_hs>>(entity);
        }
    });
    
    
    for(int x = -render_chunks; x<=render_chunks; x++) {
        for(int z = -render_chunks; z<=render_chunks; z++) {
            ChunkC chunk;
            chunk.lod = 0;
            chunk.pos = glm::vec3(x + std::round(cam.pos.x/chunk::base_length), 0, z + std::round(cam.pos.z/chunk::base_length));
            
            if(glm::distance2(glm::vec3(chunk.getPosition()), cam.pos) < Util::c_sq(render_distance + chunk::base_length)
                && map.get(chunk.pos.x, chunk.pos.z) == entt::null) {
                
                auto entity = make_chunk(reg, chunk.pos.x, chunk.pos.z);
                if(entity == entt::null) return;
                map.set(chunk.pos.x, chunk.pos.z, entity);
            }
            
        }
    }
    
}
