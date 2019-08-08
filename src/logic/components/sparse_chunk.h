#ifndef SPARSE_CHUNK_H
#define SPARSE_CHUNK_H

#include <functional>
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>

#include <cmath>
#include "global_chunk_data.h"

class SparseChunk {
    
    class Node {
    public:
        
        Node() {
            
        }
        
        void get(SparseChunk* tree, GlobalChunkData& cd, int x, int y, int z) {
            
            if(hsize == 0) {
                
                if(x < chunk::max_num_values.x && y < chunk::max_num_values.y && z < chunk::max_num_values.z) {
                    cd.data[x * chunk::max_num_values.y * chunk::max_num_values.z + z * chunk::max_num_values.y + y] = value;
                }
                
            } else if(ptrs.get<int>(0) == 0) {
                
                for(int zi = 0; zi<hsize*2; zi++) {
                    for(int yi = 0; yi<hsize*2; yi++) {
                        for(int xi = 0; xi<hsize*2; xi++) {
                            if(x + xi < chunk::max_num_values.x && y + yi < chunk::max_num_values.y && z + zi < chunk::max_num_values.z) {
                                cd.data[(x+xi) * chunk::max_num_values.y * chunk::max_num_values.z + (z+zi) * chunk::max_num_values.y + (y+yi)] = value;
                            }
                        }
                    }
                }
                
            } else {
                
                int index = 0;
                for(int zi = 0; zi<2; zi++) {
                    for(int yi = 0; yi<2; yi++) {
                        for(int xi = 0; xi<2; xi++) {
                            tree->nodes[ptrs.get<int>(index)].get(tree, cd, x + xi*hsize, y + yi*hsize, z + zi*hsize);
                            index++;
                        }
                    }
                }
            }
            
        }
        
        template <class Archive>
        void serialize( Archive & ar ) {
            ar(value, ptrs);
        }
        
        float value;
        Util::DiTypedArray<int, float, 8> ptrs;
        int hsize;
        
    };
    
public:
    
    SparseChunk() {
        
    }
    
    void get(GlobalChunkData& data) {
        nodes[root].get(this, data, 0, 0, 0);
    }
    
    void set(GlobalChunkData& cd) {
        nodes.clear();
        nodes.reserve(50);
        index = 0;
        root = make_node();
        set(root, cd, 0, 0, 0, ((int) std::pow(2, std::ceil(std::log2(chunk::num_values.x))))/2.);
        
        std::cout << chunk::max_num_values.x * chunk::max_num_values.y * chunk::max_num_values.z * sizeof(float) << " " << (index+1) << " " << nodes.capacity() * sizeof(Node) << std::endl;
    }
    
    int make_node() {
        int i = index;
        if(index == nodes.size()) {
            index++;
            nodes.push_back(Node());
        } else if(index < nodes.size()) {
            index++;
            nodes[i] = Node();
        } else {
            std::cout << "oof" << std::endl;
        }
        return i;
    }
    
    void set(int ind, GlobalChunkData& cd, int x, int y, int z, int hsize) {
        
        nodes[ind].hsize = hsize;
        
        if(hsize > 0) {
            
            float sum = 0.f;
            bool sign;
            bool empty = true;
            int i = 0;
            for(int zi = 0; zi<2; zi++) {
                for(int yi = 0; yi<2; yi++) {
                    for(int xi = 0; xi<2; xi++) {
                        nodes[ind].ptrs.get<int>(i) = make_node();
                        set(nodes[ind].ptrs.get<int>(i), cd, x + xi*hsize, y + yi*hsize, z + zi*hsize, hsize/2);
                        
                        float val = nodes[nodes[ind].ptrs.get<int>(i)].value;
                        sum += val;
                        if(i == 0) {
                            sign = std::signbit(val);
                        } else if(sign != std::signbit(val) || nodes[nodes[ind].ptrs.get<int>(i)].ptrs.get<int>(0) != 0) {
                            empty = false;
                        }
                        i++;
                    }
                }
            }
            
            nodes[ind].value = sum/8.f;
            if(empty) {
                set_index(nodes[ind].ptrs.get<int>(0));
                nodes[ind].ptrs.get<int>(0) = 0;
            }
            
        } else {
            
            if(x < chunk::max_num_values.x && y < chunk::max_num_values.y && z < chunk::max_num_values.z) {
                nodes[ind].value = cd.data[x * chunk::max_num_values.y * chunk::max_num_values.z + z * chunk::max_num_values.y + y];
            } else {
                nodes[ind].value = -1.f;
            }
            nodes[ind].ptrs.get<int>(0) = 0;
            
        }
    }
    
    void set_index(int i) {
        index = i;
    }
    
    template <class Archive>
    void serialize( Archive & ar ) {
        ar(x0, y0, z0, root, nodes);
    }
    
    std::vector<Node> nodes;
    
private:
    int root = 0;
    
    int x0 = 0;
    int y0 = 0;
    int z0 = 0;
    
    int index = 0;

};

#endif
