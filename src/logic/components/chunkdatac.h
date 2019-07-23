#ifndef CHUNKDATAC_H
#define CHUNKDATAC_H

#include "chunkc.h"

struct ChunkData {
    float values[chunk::num_values.x * chunk::num_values.y * chunk::num_values.z];
};

class ChunkDataC {
public:
    ChunkData* data = nullptr;
    int index = 0;
};

#endif
