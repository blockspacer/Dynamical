#ifndef TRANSFER_H
#define TRANSFER_H

#include <array>
#include <vulkan/vulkan.hpp>

#include "vmapp.h"

class Device;

class ImageTarget {
public:
    ImageTarget(vk::Image image, int num_components = 4, int base_mip = 0, int base_array = 0) : image(image), num_components(num_components), base_mip(base_mip), base_array(base_array) {};
    vk::Image image;
    int num_components;
    int base_mip;
    int base_array;
};

class Transfer {
public:
    Transfer(Device& device);
    void flush();
    vk::CommandBuffer getCommandBuffer();
    
    void prepareImage(std::string str, ImageTarget image);
    
    ~Transfer();
private:
    Device& device;
    
    vk::CommandPool pool;
    std::array<vk::CommandBuffer, 2> commandBuffers;
    vk::Fence fence;
    
    int index = 0;
    bool empty = true;
    
    std::array<std::vector<VmaBuffer>, 2> stagingBuffers;
    
};

#endif
