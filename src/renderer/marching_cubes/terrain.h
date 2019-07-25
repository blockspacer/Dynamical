#ifndef TERRAIN_H
#define TERRAIN_H

#include "renderer/vmapp.h"
#include "glm/glm.hpp"
#include "entt/entt.hpp"

#include "chunk.h"
#include "logic/components/chunkc.h"

#include "logic/systems/system.h"

#include "renderer/num_frames.h"

#include <stack>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
};

struct Triangle {
    Vertex a,b,c;
};

class Device;

class Terrain : System {
public:
    Terrain(Device& device);
    void preinit(entt::registry& reg) override;
    void init(entt::registry& reg) override;
    void tick(entt::registry& reg) override;
    const char* name() override {return "Terrain";};
    ~Terrain();
    
    void construction(entt::registry& reg, entt::entity entity, ChunkC& chunk);
    void destructionChunk(entt::registry& reg, entt::entity entity);
    void destructionChunkBuild(entt::registry& reg, entt::entity entity);
    void allocate(entt::registry& reg, Chunk& chonk, ChunkBuild& build);
    void deallocate(Chunk& chonk);
    void deallocate(ChunkBuild& build);
    
    vk::DescriptorSetLayout getDescLayout() {
        return descLayout;
    }
    
private:
    Device& device;
    
    VmaBuffer make_triangles(uint32_t numTriangles);
    VmaBuffer make_indirect(uint32_t numIndirect);
    
    std::array<VmaBuffer, NUM_FRAMES> chunkData;
    
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
