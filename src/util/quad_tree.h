#ifndef QUAD_TREE_H
#define QUAD_TREE_H

#include <functional>

template <typename T, T nullv>
class QuadTree {
    
    class Node {
    public:
        
        Node(int size) : hsize(size) {
            for(int i = 0; i<4; i++) {
                ptrs[i] = nullptr;
            }
        }
        
        Node(T value) : hsize(0), value(value) {}
        
        T get(int x, int y) {
            if(hsize > 0) {
                int index = (x>=hsize?1:0) + (y>=hsize?2:0);
                if(ptrs[index] == nullptr) {
                    return nullv;
                }
                return ptrs[(x>=hsize?1:0) + (y>=hsize?2:0)]->get(x-(x>=hsize?hsize:0), y-(y>=hsize?hsize:0));
            } else {
                return value;
            }
        }
    
        void set(int x, int y, T val) {
            if(hsize > 0) {
                int index = (x>=hsize?1:0) + (y>=hsize?2:0);
                if(ptrs[index] == nullptr) {
                    ptrs[index] = std::make_unique<Node>(hsize/2);
                }
                ptrs[index]->set(x-(x>=hsize?hsize:0), y-(y>=hsize?hsize:0), val);
            } else {
                value = val;
            }
        }
        
        void set(int index, std::unique_ptr<Node>& val) {
            ptrs[index] = std::move(val);
        }

        int hsize;
        
    private:
        
        T value;
        std::unique_ptr<Node> ptrs[4];
    };
    
public:
    QuadTree() {
        root = std::make_unique<Node>(nullv);
    }
    
    T get(int x, int y) {
        if(std::max(x+x0, y+y0) >= root->hsize*2 || std::min(x+x0, y+y0) < 0) return nullv;
        return root->get(x+x0, y+y0);
    }
    
    void set(int x, int y, T val) {
        
        if(std::max(x+x0, y+y0) >= root->hsize*2 || std::min(x+x0, y+y0) < 0) { // Vérifier que l'arbre est assez grand.
            
            std::unique_ptr<Node> node = std::make_unique<Node>((int) std::max(root->hsize*2, 1));
            
            node->set(((x+x0)<0?1:0) + ((y+y0)<0?2:0), root); 
            root = std::move(node); // Remplacer root par un Node plus grand.
            
            x0 += (x+x0<0?root->hsize:0);
            y0 += (y+y0<0?root->hsize:0);
            
            set(x, y, val); // Recommencer tant que l'arbre n'est pas assez grand.
        } else root->set(x+x0, y+y0, val);
        
    }
    
    void print() {
        
        for(int y = 0; y < root->hsize*2; y++) {
            for(int x = 0; x < root->hsize*2; x++) {
                if(x == x0 && y == y0) {
                    std::cout << "- ";
                } else {
                    T a = root->get(x, y);
                    std::cout << (a == nullv ? 0 : 1) << " ";
                }
            }
            std::cout << std::endl;
        }
        
    }
    
private:
    std::unique_ptr<Node> root;
    int x0;
    int y0;
        

};

#endif
