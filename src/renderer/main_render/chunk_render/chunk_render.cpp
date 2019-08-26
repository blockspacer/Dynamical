#include "chunk_render.h"

#include "renderer/marching_cubes/chunk.h"
#include "renderer/marching_cubes/terrain.h"

ChunkRender::ChunkRender(Device& device, Transfer& transfer, Swapchain& swap, Renderpass& renderpass, UBODescriptor& ubo) : chunk_pipeline(device, transfer, swap, renderpass, ubo), grass_pipeline(device, transfer, swap, renderpass, ubo), device(device), transfer(transfer), swap(swap), renderpass(renderpass) {
    
}

void ChunkRender::render(entt::registry& reg, vk::CommandBuffer command, vk::DescriptorSet set) {
    
    Chunk::mutex.lock();
    command.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, chunk_pipeline, 0, {set}, {});
    
    command.bindPipeline(vk::PipelineBindPoint::eGraphics, chunk_pipeline);
    
    command.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, chunk_pipeline, 1, {chunk_pipeline.materialSet}, {});
    
    reg.view<Chunk, entt::tag<"ready"_hs>>().each([&](Chunk& chonk, auto) {
        
        command.bindVertexBuffers(0, {chonk.triangles}, {chonk.triangles_offset * sizeof(Triangle)});
        
        command.drawIndirect(chonk.indirect, chonk.indirect_offset * sizeof(vk::DrawIndirectCommand), 1, 0);
        
    });
    
    command.bindPipeline(vk::PipelineBindPoint::eGraphics, grass_pipeline);
    
    command.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, grass_pipeline, 1, {grass_pipeline.descSet}, {});
    
    reg.view<Chunk, entt::tag<"ready"_hs>>().each([&](Chunk& chonk, auto) {
        
        command.bindVertexBuffers(0, {chonk.triangles}, {chonk.triangles_offset * sizeof(Triangle)});
        
        command.drawIndirect(chonk.indirect, chonk.indirect_offset * sizeof(vk::DrawIndirectCommand), 1, 0);
        
    });
    
    Chunk::mutex.unlock();
    
}

ChunkRender::~ChunkRender() {
    
}
