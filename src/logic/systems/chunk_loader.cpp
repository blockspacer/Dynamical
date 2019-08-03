#include "system_list.h"
#include "logic/components/chunk_map.h"

void ChunkLoaderSys::init() {
    reg.set<GlobalChunkMap>();
}

void ChunkLoaderSys::tick() {
    
}

