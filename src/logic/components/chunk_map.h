#ifndef CHUNK_MAP_H
#define CHUNK_MAP_H

#include "util/quad_tree.h"

#include "entt/entt.hpp"

constexpr int map_chunk_height = 5;

class ChunkMap {
public:
    
    entt::entity get(int x, int y, int z) {
        return tree.get(x, y, z);
    }
    
    void set(int x, int y, int z, entt::entity entity) {
        tree.set(x, y, z, entity);
    }
    void remove(int x, int y, int z) {
        set(x, y, z, entt::null);
    }
    void print() {
        tree.print();
    }
    
private:
    QuadTree<entt::entity, entt::null> tree;
    
};

#endif
