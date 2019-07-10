#include "system_list.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/norm.hpp"

#include "logic/components/chunkc.h"
#include "util/util.h"

#include "logic/components/chunk_map.h"
#include "logic/components/camerac.h"
#include "renderer/camera.h"

constexpr float render_chunks = render_distance / chunk_base_length + 1;

entt::entity make_chunk(entt::registry& reg, int x, int z) {
    
    auto entity = reg.create();
    ChunkC& chunk = reg.assign<ChunkC>(entity);
    chunk.pos = glm::vec3(x, 0, z);
    chunk.lod = 0;
    return entity;
    
}

void ChunkSys::init(entt::registry& reg) {
    
    ChunkMap& map = reg.set<ChunkMap>();
    
}


void ChunkSys::tick(entt::registry& reg) {
    
    ChunkMap& map = reg.ctx<ChunkMap>();
    CameraC& cam = reg.ctx<CameraC>();
    
    reg.view<ChunkC>().each([&](entt::entity entity, ChunkC& chunk) {
        if(glm::distance2(chunk.getPosition(), cam.pos) > Util::c_sq(render_distance + chunk_base_length*2)) {
            map.remove(chunk.pos.x, chunk.pos.z);
            if(!reg.has<entt::tag<"destroying"_hs>>(entity)) reg.assign<entt::tag<"destroying"_hs>>(entity);
        }
    });
    
    
    for(int x = -render_chunks; x<=render_chunks; x++) {
        for(int z = -render_chunks; z<=render_chunks; z++) {
            ChunkC chunk;
            chunk.lod = 0;
            chunk.pos = glm::vec3(x + std::round(cam.pos.x/chunk_base_length), 0, z + std::round(cam.pos.z/chunk_base_length));
            
            if(glm::distance2(chunk.getPosition(), cam.pos) < Util::c_sq(render_distance + chunk_base_length)
                && map.get(chunk.pos.x, chunk.pos.z) == entt::null) {
                
                map.set(chunk.pos.x, chunk.pos.z, make_chunk(reg, chunk.pos.x, chunk.pos.z));
            }
            
        }
    }
    
}
