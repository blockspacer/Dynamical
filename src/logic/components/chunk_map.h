#ifndef CHUNK_MAP_H
#define CHUNK_MAP_H

#include "util/quad_tree.h"

#include "entt/entt.hpp"

constexpr int map_chunk_height = 5;

class ChunkMap {
public:
    
    class Tower {
    public:
        Tower() {
            for(int i = 0; i<map_chunk_height; i++) {
                arr[i] = entt::null;
            }
        }
        entt::entity arr[map_chunk_height];
    };
    
    entt::entity* get(int x, int z) {
        Tower* arr = tree.get(x, z);
        if(arr == nullptr) return nullptr;
        return reinterpret_cast<entt::entity*> (arr);
    }
    entt::entity get(int x, int y, int z) {
        entt::entity* arr = get(x,z);
        if(arr == nullptr) return entt::null;
        return arr[y];
    }
    void set(int x, int y, int z, entt::entity entity) {
        entt::entity* arr = get(x,z);
        if(arr == nullptr) {
            arr = reinterpret_cast<entt::entity*> (tree.set(x, z, Tower()));
        }
        arr[y] = entity;
    }
    void remove(int x, int y, int z) {
        set(x, y, z, entt::null);
    }
    void print() {
        tree.print();
    }
    
private:
    QuadTree<Tower> tree;
    
};

#endif
