#include "system_list.h"

#include "logic/components/chunkc.h"
#include "renderer/num_frames.h"

#include "logic/components/chunk_map.h"

entt::entity make_chunk(entt::registry& reg, int x, int z) {
    
    auto entity = reg.create();
    ChunkC& chunk = reg.assign<ChunkC>(entity);
    chunk.pos = 40.f * glm::vec3(x, 0, z);
    chunk.cubeSize = 2.f;
    chunk.gridSize = glm::vec3(40, 32, 40);
    return entity;
    
}

const int chunks_num = 5;
static int chunks_index = 0;
static int chunks_x[chunks_num];
static int chunks_y[chunks_num];

void ChunkSys::init(entt::registry& reg) {
    
    for(int i=0; i<chunks_num; i++) {
        chunks_x[i] = 0;
        chunks_y[i] = 0;
    }
    
    reg.set<ChunkMap>();
    
}


void ChunkSys::tick(entt::registry& reg) {
    
    static int index = 0;
    
    if(index%20 == 0) {
        
        ChunkMap& map = reg.ctx<ChunkMap>();
        
        entt::entity ent = map.get(chunks_x[chunks_index], chunks_y[chunks_index]);
        if(ent != entt::null) {
            reg.destroy(ent);
            map.remove(chunks_x[chunks_index], chunks_y[chunks_index]);
        }
        
        chunks_x[chunks_index] = (index/20)%4;
        chunks_y[chunks_index] = (index/20)/4;
        map.set(chunks_x[chunks_index], chunks_y[chunks_index], make_chunk(reg, chunks_x[chunks_index], chunks_y[chunks_index]));
        
        chunks_index = (chunks_index+1)%chunks_num;
        
    }
    
    index++;
    
}
