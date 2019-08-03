#include "system_list.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/norm.hpp"

#include <util/util.h>
#include <logic/components/chunkc.h>
#include <logic/components/chunk_map.h>
#include <logic/components/camerac.h>
#include <renderer/camera.h>
#include "logic/components/global_chunk_data.h"

void ChunkManagerSys::init() {
    reg.set<ChunkMap>();
}

void ChunkManagerSys::tick() {
    
    ChunkMap& map = reg.ctx<ChunkMap>();
    GlobalChunkMap& global_map = reg.ctx<GlobalChunkMap>();
    CameraC cam = reg.ctx<CameraC>();
    
    ChunkC chunk;
    const int lod = chunk::max_lod;
    chunk.lod = lod;
    const int mul = chunk.getLOD();
    const int render_chunks = render_distance / (chunk.getSize().x)+1;
    reg.view<ChunkC>().each([&](entt::entity entity, ChunkC& chunk) {
        if(glm::distance2(glm::vec3(chunk.getPosition()), cam.pos) > Util::sq(render_distance + chunk.getSize().x*2)) {
            map.set(entt::null, chunk.pos.x, chunk.pos.y, chunk.pos.z, mul/2);
            if(!reg.has<entt::tag<"destroying"_hs>>(entity)) reg.assign<entt::tag<"destroying"_hs>>(entity);
        }
    });
    
    for(int x = -render_chunks; x<=render_chunks; x++) {
        for(int z = -render_chunks; z<=render_chunks; z++) {
            for(int y = -render_chunks; y<=render_chunks; y++) {
                
                chunk.pos = glm::vec3(x + std::round(cam.pos.x/(mul*chunk::base_length)), y + std::round(cam.pos.y/(mul*chunk::base_length)), z + std::round(cam.pos.z/(mul*chunk::base_length))) * (float) mul;
                
                if(map.get(chunk.pos.x, chunk.pos.y, chunk.pos.z, mul/2) == entt::null && glm::distance2(glm::vec3(chunk.getPosition()), cam.pos) < Util::sq(render_distance + chunk.getSize().x)) {
                    
                    auto chunk_entity = reg.create();
                    ChunkC& cc = reg.assign<ChunkC>(chunk_entity);
                    cc.pos = chunk.pos;
                    cc.lod = chunk.lod;
                    map.set(chunk_entity, chunk.pos.x, chunk.pos.y, chunk.pos.z, mul/2);
                    
                    entt::entity global_entity = global_map.get(chunk.pos.x, chunk.pos.y, chunk.pos.z, chunk::max_mul/2);
                    if(global_entity == entt::null) {
                        global_entity = reg.create();
                        reg.assign<GlobalChunkC>(global_entity, (chunk.pos/mul)*mul, chunk::max_lod);
                        global_map.set(global_entity, chunk.pos.x, chunk.pos.y, chunk.pos.z, chunk::max_mul/2);
                    }
                    if(!reg.has<GlobalChunkData>(global_entity))
                        reg.assign<entt::tag<"loading"_hs>>(global_entity);
                    
                    reg.assign<OuterGlobalChunk>(chunk_entity, global_entity);
                    reg.assign<entt::tag<"preparing"_hs>>(chunk_entity);
                    
                }
                
            }
        }
    }
    
}
