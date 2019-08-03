#ifndef CHUNK_MAP_H
#define CHUNK_MAP_H

#include "util/quad_tree.h"

#include "entt/entt.hpp"

class ChunkMap {
public:
    
    entt::entity get(int x, int y, int z, int lod = 0) {
        return tree.get(x, y, z, lod);
    }
    
    void set(entt::entity entity, int x, int y, int z, int lod = 0) {
        tree.set(x, y, z, lod, entity);
    }
    void remove(int x, int y, int z) {
        tree.remove(x, y, z);
    }
    void print() {
        tree.print();
    }
    
    template <class Archive>
    void serialize( Archive & ar ) {
        ar(tree);
    }
    
private:
    QuadTree<entt::entity, entt::null> tree;
    
};

class GlobalChunkMap : public ChunkMap {
    
};

#endif
