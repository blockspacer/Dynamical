#include "terrain.h"

#include "renderer/device.h"

#include "logic/components/chunkc.h"
#include "logic/components/positionc.h"
#include "chunk.h"

#include <iostream>

constexpr uint32_t NUM_TRIANGLES = 30000/3;
constexpr uint32_t NUM_INDIRECT = 10;
constexpr uint32_t MAX_CHUNKS = 5000;

Terrain::Terrain(Device& device) : device(device) {
    
    auto poolSizes = std::vector {
        vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, MAX_CHUNKS*2)
    };
    descPool = device->createDescriptorPool(vk::DescriptorPoolCreateInfo(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, MAX_CHUNKS, poolSizes.size(), poolSizes.data()));
    
    auto bindings = std::vector {
        vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute),
        vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute),
    };
    descLayout = device->createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo({}, bindings.size(), bindings.data()));
    
    triangles.reserve(MAX_CHUNKS);
    indirects.reserve(NUM_INDIRECT);
    
    indirects.push_back(IndirectAllocation(make_indirect(NUM_INDIRECT), NUM_INDIRECT));
    
}

void Terrain::preinit(entt::registry& reg) {
    
    reg.on_construct<ChunkC>().connect<&Terrain::construction>(this);
    reg.on_destroy<Chunk>().connect<&Terrain::destruction>(this);
    
}

void Terrain::init(entt::registry& reg) {
    
}

void Terrain::construction(entt::registry& reg, entt::entity entity, ChunkC& chunk) {
    
    reg.assign<Chunk>(entity);
    reg.assign<entt::tag<"modified"_hs>>(entity);
    
}

void Terrain::destruction(entt::registry& reg, entt::entity entity) {
    
    deallocate(reg.get<Chunk>(entity));
    
}

void Terrain::allocate(Chunk& chonk) {
    
    if(!indirectSlots.empty()) {
        const auto& slot = indirectSlots.top();
        chonk.indirect = indirects[slot.allocIndex].buffer;
        chonk.indirect_offset = slot.index;
        indirectSlots.pop();
    } else {
        auto& alloc = indirects.back();
        if(alloc.num < alloc.max) {
            chonk.indirect = alloc.buffer;
            chonk.indirect_offset = alloc.num;
            alloc.num++;
        } else {
            indirects.push_back(IndirectAllocation(make_indirect(NUM_INDIRECT), NUM_INDIRECT));
            auto& new_alloc = indirects.back();
            chonk.indirect = new_alloc.buffer;
            chonk.indirect_offset = new_alloc.num;
            new_alloc.num++;
        }
    }
    
    if(!triangleSlots.empty()) {
        const auto& slot = triangleSlots.top();
        chonk.triangles = triangles[slot];
        triangleSlots.pop();
    } else {
        //std::cout << (triangles.size() * NUM_TRIANGLES * sizeof(Triangle)) / 1000000. << std::endl;
        triangles.push_back(make_triangles(NUM_TRIANGLES));
        chonk.triangles = triangles.back();
    }
    chonk.triangles_offset = 0;
    
    chonk.set = device->allocateDescriptorSets(vk::DescriptorSetAllocateInfo(descPool, 1, &descLayout))[0];
    
    auto triInfo = vk::DescriptorBufferInfo(chonk.triangles, 0, NUM_TRIANGLES * sizeof(Triangle));
    auto indInfo = vk::DescriptorBufferInfo(chonk.indirect, chonk.indirect_offset * sizeof(vk::DrawIndirectCommand), sizeof(vk::DrawIndirectCommand));
    
    device->updateDescriptorSets({
        vk::WriteDescriptorSet(chonk.set, 0, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &triInfo, nullptr),
        vk::WriteDescriptorSet(chonk.set, 1, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &indInfo, nullptr),
    }, {});
    
}

void Terrain::deallocate(Chunk& chonk) {
    
    device->freeDescriptorSets(descPool, chonk.set);
    
    for(uint32_t i = 0; i < indirects.size(); i++) {
        if(chonk.indirect == indirects[i].buffer) {
            indirectSlots.push(IndirectSlot{i, chonk.indirect_offset});
            break;
        }
    }
    
    for(uint32_t i = 0; i < triangles.size(); i++) {
        if(chonk.triangles == triangles[i].buffer) {
            triangleSlots.push(i);
            break;
        }
    }
    
}


void Terrain::tick(entt::registry& reg) {
    
    reg.view<ChunkC, Chunk, entt::tag<"modified"_hs>>().each([&](entt::entity entity, ChunkC& chunk, Chunk& chonk, auto) {
        
        if(chonk.set) {
            deallocate(chonk);
        }
        
        allocate(chonk);
        
    });
    
}

Terrain::~Terrain() {
    
    device->destroy(descLayout);
    
    device->destroy(descPool);
    
}

VmaBuffer Terrain::make_triangles(uint32_t numTriangles) {
    
    VmaAllocationCreateInfo info = {};
    info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    
    bool concurrent = (device.g_i != device.c_i);
    uint32_t qfs[2] = {device.g_i, device.c_i};
    
    return dy::make_buffer(device, &info, vk::BufferCreateInfo(
        {}, numTriangles * sizeof(Triangle),
        vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eVertexBuffer,
        concurrent ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive, concurrent ? 2 : 1, &qfs[0]));
}

VmaBuffer Terrain::make_indirect(uint32_t numIndirect) {
    
    VmaAllocationCreateInfo info = {};
    info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    
    bool concurrent = (device.g_i != device.c_i);
    uint32_t qfs[2] = {device.g_i, device.c_i};
    
    return dy::make_buffer(device, &info, vk::BufferCreateInfo(
        {}, numIndirect * sizeof(vk::DrawIndirectCommand), 
        vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eIndirectBuffer,
        concurrent ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive, concurrent ? 2 : 1, &qfs[0]));
    
}

