#include <iostream>

#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/norm.hpp"

#include <iostream>
#include <fstream>

#include "cereal/archives/portable_binary.hpp"

#include "../include/raycast_data.h"

#define _USE_MATH_DEFINES
#include <cmath>

constexpr int num_objects = 8;
constexpr double object_size = 0.2;

glm::u8vec4 cast(double a, double x, double y) {
    
    glm::dvec2 pos(x, y);
    glm::dvec2 dir(std::cos(a+0.00001), std::sin(a+0.00001));
    glm::dvec2 tangent(-dir.y, dir.x);
    
    double min = 1000.;
    glm::dvec2 norm(0., 0.);
    
    for(int cx = 0; cx < num_objects; cx++) {
        for(int cy = 0; cy < num_objects; cy++) {
            
            glm::dvec2 cpos((cx + 0.5) * tile_size / num_objects, (cy + 0.5) * tile_size / num_objects);
            
            double t2 = (pos.x - cpos.x + (cpos.y - pos.y) * (dir.x/dir.y)) /
                        (tangent.x * (1 + (dir.x*dir.x)/(dir.y*dir.y)));
            
            if(std::abs(t2) > object_size) continue;
            
            double t1 = (cpos.y - pos.y + tangent.y * t2) / dir.y;
            
            if(t1 < min && t1 >= 0) {
                min = t1;
                glm::dvec2 intersection = pos + dir * t1;
                norm = glm::normalize(intersection - cpos);
            }
            
        }
    }
    
    return glm::u8vec4(1./(1.+min) * 255, norm.x * 255, 0, norm.y * 255);
}

void raycast(const char* output_file) {
    
    RaycastData data;
    
    for(int a = 0; a<num_angles; a++) {
        for(int z = 0; z<num_samples; z++) {
            for(int x = 0; x<num_samples; x++) {
                
                data.data[a * num_samples * num_samples + z * num_samples + x] =
                cast(a * 2. * M_PI / num_angles, (double) x * tile_size / num_samples, (double) z * tile_size / num_samples);
                
            }
        }
    }
    
    std::ofstream os(output_file);
    cereal::PortableBinaryOutputArchive out(os);
    
    out(data);
    
}


int main(int argc, char **argv) {
    
    if(argc == 2) {
        
        raycast(argv[1]);
        
        
        
        
        return 0;
    }
    
    std::cout << "Proper use : raycast output_file" << std::endl;
    return 1;
}
