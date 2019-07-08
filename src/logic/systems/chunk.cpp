#include "system_list.h"

#include "logic/components/chunkc.h"
#include "renderer/num_frames.h"

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
static entt::entity chunks[chunks_num];

void ChunkSys::init(entt::registry& reg) {
    
    /*
    for(int x = -3; x<4; x++) {
        for(int z = -3; z<4; z++) {
            
            auto entity = reg.create();
            ChunkC& chunk = reg.assign<ChunkC>(entity);
            chunk.pos = 40.f * glm::vec3(x, 0, z);
            chunk.cubeSize = 2.f;
            chunk.gridSize = glm::vec3(40, 32, 40);
            
        }
    }
    */
    
    for(int i=0; i<chunks_num; i++) {
        chunks[i] = entt::null;
    }
    
}


void ChunkSys::tick(entt::registry& reg) {
    
    static int index = 0;
    
    if(index%20 == 0) {
        
        if(chunks[chunks_index] != entt::null) reg.destroy(chunks[chunks_index]);
        
        chunks[chunks_index] = make_chunk(reg, (index/20)%4, (index/20)/4);;
        
        chunks_index = (chunks_index+1)%chunks_num;
        
        
    }
    
    index++;
    
}
