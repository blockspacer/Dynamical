#include "transfer.h"

#include "device.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

Transfer::Transfer(Device& device) : device(device) {
    
    pool = device->createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer, device.t_i));
    
    vk::CommandBufferAllocateInfo info(pool, vk::CommandBufferLevel::ePrimary, 2);
    device->allocateCommandBuffers(&info, commandBuffers.data());
    
    fence = device->createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
    
    commandBuffers[index].begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    
}

void Transfer::flush() {
    
    device->waitForFences(fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
    
    if(!empty) {
        
        commandBuffers[index].end();
        
        device->resetFences(fence);
        
        device.transfer.submit(vk::SubmitInfo(0, nullptr, nullptr, 1, &commandBuffers[index], 0, nullptr), fence);
        
        index = (index+1)%2;
        
        stagingBuffers[index].clear();
        
        commandBuffers[index].begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
        
        empty = true;
        
    }
    
}

void Transfer::prepareImage(std::string str, ImageTarget image) {
    
    int x, y, channels;
    stbi_uc* data = stbi_load(str.c_str(), &x, &y, &channels, image.num_components);
    
    if(data == nullptr) {
        std::cout << "image " << str << " could not be loaded because : " << stbi_failure_reason() << std::endl;
    }
    
    VmaAllocationCreateInfo info {};
    info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    info.usage = VMA_MEMORY_USAGE_CPU_ONLY;
    stagingBuffers[index].push_back(VmaBuffer(device, &info, vk::BufferCreateInfo(
        {}, x * y * image.num_components * sizeof(stbi_uc), vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive, 1, &device.t_i
    )));
    const VmaBuffer& stagingBuffer = stagingBuffers[index].back();
    
    VmaAllocationInfo inf;
    vmaGetAllocationInfo(device, stagingBuffer.allocation, &inf);
    
    memcpy(inf.pMappedData, data, x * y * image.num_components * sizeof(stbi_uc));
    
    stbi_image_free(data);
    
    vk::CommandBuffer buffer = getCommandBuffer();
    
    buffer.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlagBits::eByRegion, {}, {}, vk::ImageMemoryBarrier(
        {}, vk::AccessFlagBits::eTransferWrite, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
        0, 0, image.image, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, image.base_mip, 1, image.base_array, 1)
    ));
    
    buffer.copyBufferToImage(stagingBuffer.buffer, image.image, vk::ImageLayout::eTransferDstOptimal,
        vk::BufferImageCopy(0, x, y, vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, image.base_mip, image.base_array, 1), vk::Offset3D(0, 0, 0), vk::Extent3D(x, y, 1)));
    
    buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, vk::DependencyFlagBits::eByRegion, {}, {}, vk::ImageMemoryBarrier(
        vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead,
        vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
        0, 0, image.image, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, image.base_mip, 1, image.base_array, 1)
    ));
    
}

vk::CommandBuffer Transfer::getCommandBuffer() {
    empty = false;
    return commandBuffers[index];
}

Transfer::~Transfer() {
    
    for(int i = 0; i<2; i++) {
        stagingBuffers[i].clear();
    }
    
    device->destroy(fence);
    
    device->destroy(pool);
    
}
