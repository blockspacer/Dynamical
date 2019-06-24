#ifndef VMA_H
#define VMA_H

#include "vma/vk_mem_alloc.h"
#include "vulkan/vulkan.hpp"

#include <iostream>

class Device;

class VmaBuffer {
public:
    
    VmaBuffer();
    
    VmaBuffer(Device& device, VmaAllocationCreateInfo* allocInfo, const vk::BufferCreateInfo& bufferInfo);
    
    ~VmaBuffer();
    
    operator VmaAllocation() { return allocation; };
    operator vk::Buffer() { return buffer; };
    VmaBuffer& operator= (VmaBuffer&& buffer) {
        this->device = buffer.device;
        this->allocation = buffer.allocation;
        this->buffer = buffer.buffer;
        this->size = buffer.size;
        this->offset = buffer.offset;
        this->memory = buffer.memory;
        buffer.device = nullptr;
        return *this;
    };
    
    VmaAllocation allocation;
    vk::Buffer buffer;
    
    vk::DeviceSize size;
    vk::DeviceSize offset;
    vk::DeviceMemory memory;
    
private:
    Device* device;
};

class VmaImage {
public:
    
    VmaImage();
    
    VmaImage(Device& device, VmaAllocationCreateInfo* allocInfo, const vk::ImageCreateInfo& imageInfo);
    
    ~VmaImage();
    
    operator VmaAllocation() { return allocation; }
    operator vk::Image() { return image; }
    VmaImage& operator= (VmaImage&& image) {
        this->~VmaImage();
        this->device = image.device;
        this->allocation = image.allocation;
        this->image = image.image;
        this->size = image.size;
        this->offset = image.offset;
        this->memory = image.memory;
        image.device = nullptr;
        return *this;
    }
    
    VmaAllocation allocation;
    vk::Image image;
    
    vk::DeviceSize size;
    vk::DeviceSize offset;
    vk::DeviceMemory memory;
    
private:
    Device* device;
};

#endif
