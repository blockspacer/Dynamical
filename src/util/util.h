#ifndef UTIL_H
#define UTIL_H

#include <vector>

namespace Util {
    
    std::vector<char> readFile(char const* filename);
    
    template<typename T>
    std::vector<T> nTimes(int n, T t) {
        std::vector<T> vec(n);
        for(int i = 0; i<n; i++) {
            vec[i] = t;
        }
        return vec;
    }
    
    template<typename T>
    int makeSet(std::vector<T> vec) {
        int num = 1;
        
        for(int i = 1; i < vec.size(); i++) {
            bool unique = true;
            
            for(int j = 0; j < num; j++) {
                
                if(vec[i] == vec[j]) {
                    unique = false;
                    break;
                }
                
            }
            
            if(unique) {
                if(num != i) vec[num] = vec[i];
                num++;
            }
        }
        
        return num;
    }
    
};

#endif
