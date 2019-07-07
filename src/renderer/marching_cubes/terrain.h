#ifndef TERRAIN_H
#define TERRAIN_H

#include "renderer/vmapp.h"

#include "glm/glm.hpp"

#include "entt/entt.hpp"

#include <stack>

struct Triangle {
    glm::vec4 a,b,c;
};

class Device;
class ChunkC;

class Terrain {
public:
    Terrain(Device& device);
    void init(entt::registry& reg);
    void update(entt::registry& reg);
    ~Terrain();
    
    void construction(entt::registry& reg, entt::entity entity, ChunkC& chunk);
    void destruction(entt::registry& reg, entt::entity entity);
    
    vk::DescriptorSetLayout getDescLayout() {
        return descLayout;
    }
    
private:
    Device& device;
    
    VmaBuffer make_triangles(uint32_t numTriangles);
    VmaBuffer make_indirect(uint32_t numIndirect);
    
    std::vector<VmaBuffer> triangles;
    std::stack<uint32_t> triangleSlots;
    
    class IndirectAllocation {
    public:
        IndirectAllocation(VmaBuffer buffer, int max) : buffer(std::move(buffer)), max(max), num(0) {};
        
        VmaBuffer buffer;
        uint32_t max;
        uint32_t num;
        
        IndirectAllocation(IndirectAllocation&& other) {
            max = other.max;
            num = other.num;
            buffer = std::move(other.buffer);
        };
        
        IndirectAllocation& operator= (IndirectAllocation&& other) {
            max = other.max;
            num = other.num;
            buffer = std::move(other.buffer);
            return *this;
        };
    };
    
    struct IndirectSlot {
        uint32_t allocIndex;
        uint32_t index;
    };
    
    std::vector<IndirectAllocation> indirects;
    std::stack<IndirectSlot> indirectSlots;
    
    vk::DescriptorPool descPool;
    vk::DescriptorSetLayout descLayout;
    std::vector<vk::DescriptorSet> descSets;
    
};

#endif
