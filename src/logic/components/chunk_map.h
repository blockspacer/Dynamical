#ifndef CHUNK_MAP_H
#define CHUNK_MAP_H

#include "util/quad_tree.h"

#include "entt/entt.hpp"

class ChunkMap {
public:
    
    entt::entity get(int x, int y) {
        return tree.get(x, y);
    }
    void set(int x, int y, entt::entity entity) {
        tree.set(x, y, entity);
    }
    void remove(int x, int y) {
        tree.set(x, y, entt::null);
    }
    void print() {
        tree.print();
    }
    
private:
    QuadTree<entt::entity, entt::null> tree;
    
};

#endif
