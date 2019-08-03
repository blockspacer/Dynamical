#ifndef GLOBAL_CHUNK_H
#define GLOBAL_CHUNK_H

#include "entt/entt.hpp"

#include "chunkc.h"

#include "chunkdatac.h"

class GlobalChunkData {
public:
    GlobalChunkData() : data(std::make_unique<ChunkData>()) {};
    std::unique_ptr<ChunkData> data;
};

class GlobalChunkEmpty {
public:
    GlobalChunkEmpty(float mean) : mean(mean) {} 
    float mean;
};

#endif
