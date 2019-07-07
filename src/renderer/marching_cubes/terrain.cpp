#include "terrain.h"

#include "renderer/device.h"

#include "logic/components/chunkc.h"
#include "logic/components/positionc.h"
#include "chunk.h"

#include <iostream>

Terrain::Terrain(Device& device) : device(device) {
    
    uint32_t maxChunks = 100;
    
    auto poolSizes = std::vector {
        vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, maxChunks*2)
    };
    descPool = device->createDescriptorPool(vk::DescriptorPoolCreateInfo({}, maxChunks, poolSizes.size(), poolSizes.data()));
    
    auto bindings = std::vector {
        vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute),
        vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute),
    };
    descLayout = device->createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo({}, bindings.size(), bindings.data()));
    
    
    triangles.reserve(maxChunks);
    indirects.reserve(10);
    
    indirects.push_back(IndirectAllocation(make_indirect(10), 10));
    
}

void Terrain::init(entt::registry& reg) {
    
    reg.on_construct<ChunkC>().connect<&Terrain::construction>(this);
    reg.on_destroy<ChunkC>().connect<&Terrain::destruction>(this);
    
}

void Terrain::construction(entt::registry& reg, entt::entity entity, ChunkC& chunk) {
    
    Chunk& chonk = reg.assign<Chunk>(entity);
    
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
            indirects.push_back(IndirectAllocation(make_indirect(10), 10));
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
        triangles.push_back(make_triangles(30000));
        chonk.triangles = triangles.back();
    }
    chonk.triangles_offset = 0;
    
    chonk.set = device->allocateDescriptorSets(vk::DescriptorSetAllocateInfo(descPool, 1, &descLayout))[0];
    
    auto triInfo = vk::DescriptorBufferInfo(chonk.triangles, 0, 30000 * sizeof(Triangle));
    auto indInfo = vk::DescriptorBufferInfo(chonk.indirect, chonk.indirect_offset * sizeof(vk::DrawIndirectCommand), (10 - chonk.indirect_offset) * sizeof(vk::DrawIndirectCommand));
    
    device->updateDescriptorSets({
        vk::WriteDescriptorSet(chonk.set, 0, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &triInfo, nullptr),
        vk::WriteDescriptorSet(chonk.set, 1, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &indInfo, nullptr),
    }, {});
    
}

void Terrain::destruction(entt::registry& reg, entt::entity entity) {
    
    auto& chonk = reg.get<Chunk>(entity);
    reg.remove<Chunk>(entity);
    
}


void Terrain::update(entt::registry& reg) {
    
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
        {}, numIndirect * sizeof(VkDrawIndirectCommand), 
        vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eIndirectBuffer,
        concurrent ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive, concurrent ? 2 : 1, &qfs[0]));
    
}

