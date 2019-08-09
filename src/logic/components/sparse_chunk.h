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
        
        Node() {}
        
        void get(SparseChunk* tree, GlobalChunkData& cd, int x, int y, int z) {
            
            if(hsize == 1) {
                
                int i = 0;
                for(int zi = 0; zi<2; zi++) {
                    for(int yi = 0; yi<2; yi++) {
                        for(int xi = 0; xi<2; xi++) {
                            
                            if(x + xi < chunk::max_num_values.x && y + yi < chunk::max_num_values.y && z + zi < chunk::max_num_values.z) {
                                cd.data[(x+xi) * chunk::max_num_values.y * chunk::max_num_values.z + (z+zi) * chunk::max_num_values.y + (y+yi)]
                                = val[i++];
                            }
                            
                        }
                    }
                }
                
            } else if(empty) {
                
                for(int zi = 0; zi<hsize*2; zi++) {
                    for(int yi = 0; yi<hsize*2; yi++) {
                        for(int xi = 0; xi<hsize*2; xi++) {
                            
                            if(x + xi < chunk::max_num_values.x && y + yi < chunk::max_num_values.y && z + zi < chunk::max_num_values.z) {
                                
                                float x1 = Util::lerp(val[0], val[1], xi/(hsize*2-1));
                                float x2 = Util::lerp(val[2], val[3], xi/(hsize*2-1));
                                float x3 = Util::lerp(val[4], val[5], xi/(hsize*2-1));
                                float x4 = Util::lerp(val[6], val[7], xi/(hsize*2-1));
                                
                                float y1 = Util::lerp(x1, x2, yi/(hsize*2-1));
                                float y2 = Util::lerp(x3, x4, yi/(hsize*2-1));
                                
                                float value = Util::lerp(y1, y2, zi/(hsize*2-1));
                                
                                cd.data[(x+xi) * chunk::max_num_values.y * chunk::max_num_values.z + (z+zi) * chunk::max_num_values.y + (y+yi)]
                                = value;
                                
                            }
                            
                        }
                    }
                }
                
            } else {
                
                int index = 0;
                for(int zi = 0; zi<2; zi++) {
                    for(int yi = 0; yi<2; yi++) {
                        for(int xi = 0; xi<2; xi++) {
                            tree->nodes[ind[index++]].get(tree, cd, x + xi*hsize, y + yi*hsize, z + zi*hsize);
                        }
                    }
                }
            }
            
        }
        
        template <class Archive>
        void serialize( Archive & ar ) {
            ar(empty, ind, hsize);
        }
        
        bool empty;
        union {
            std::array<int, 8> ind;
            std::array<float, 8> val;
        };
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
        nodes.resize(32768);
        index = 0;
        root = make_node();
        set(root, cd, 0, 0, 0, ((int) std::pow(2, std::ceil(std::log2(chunk::num_values.x)))/2.));
        
        nodes.resize(index+1);
        nodes.shrink_to_fit();
        
        std::cout << chunk::max_num_values.x * chunk::max_num_values.y * chunk::max_num_values.z * sizeof(float) << " " << nodes.capacity() * sizeof(Node) << std::endl;
    }
    
    int make_node() {
        int i = index;
        if(index == nodes.size()) {
            index++;
            nodes.push_back(Node());
        } else if(index < nodes.size()) {
            index++;
            nodes[i] = Node();
        }
        return i;
    }
    
    float set(int ind, GlobalChunkData& cd, int x, int y, int z, int hsize) {
        
        nodes[ind].hsize = hsize;
        
        if(hsize > 1) {
            
            std::array<float, 8> temp_val;
            
            float sum = 0.f;
            bool sign;
            bool empty = true;
            int i = 0;
            for(int zi = 0; zi<2; zi++) {
                for(int yi = 0; yi<2; yi++) {
                    for(int xi = 0; xi<2; xi++) {
                        int index = make_node();
                        nodes[ind].ind[i] = index;
                        float val = set(nodes[ind].ind[i], cd, x + xi*hsize, y + yi*hsize, z + zi*hsize, hsize/2);
                        
                        if(val == 0.f || !empty) {
                            
                            empty = false;
                            
                        } else {
                        
                            temp_val[i] = nodes[nodes[ind].ind[i]].val[i];
                            sum += val;
                            if(i == 0) {
                                sign = std::signbit(val);
                            } else if(sign != std::signbit(val)) {
                                empty = false;
                            }
                            
                        }
                        i++;
                    }
                }
            }
            
            nodes[ind].empty = empty;
            if(empty) {
                set_index(nodes[ind].ind[0]);
                nodes[ind].val = temp_val;
                return sum/8.f;
            } else {
                return 0.f;
            }
            
        } else {
            
            float sum = 0.f;
            bool sign;
            bool empty = true;
            int i = 0;
            
            for(int zi = 0; zi<2; zi++) {
                for(int yi = 0; yi<2; yi++) {
                    for(int xi = 0; xi<2; xi++) {
                        float val = 0.f;
                        if(x + xi < chunk::max_num_values.x && y + yi < chunk::max_num_values.y && z + zi < chunk::max_num_values.z) {
                            val = cd.data[(x + xi) * chunk::max_num_values.y * chunk::max_num_values.z + (z + zi) * chunk::max_num_values.y + (y+yi)];
                        }
                        
                        sum += val;
                        if(i == 0) {
                            sign = std::signbit(val);
                        } else if(sign != std::signbit(val)) {
                            empty = false;
                        }
                        
                        nodes[ind].val[i] = val;
                        i++;
                    }
                }
            }
            
            nodes[ind].empty = empty;
            if(empty) {
                return sum/8.f;
            } else {
                return 0.f;
            }
            
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
