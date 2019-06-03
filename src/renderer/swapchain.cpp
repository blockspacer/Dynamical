#include "swapchain.h"

#include "windu.h"
#include "instance.h"
#include "device.h"
#include "loader.inl"

#include <SDL_vulkan.h>
#include <iostream>
#include <time.h>

Swapchain::Swapchain(Windu& win, Instance& instance, Device &device) : win(win), instance(instance), device(device) {
    init();
}

void Swapchain::init() {
    
    INST_LOAD(vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
    INST_LOAD(vkGetPhysicalDeviceSurfaceFormatsKHR)
    INST_LOAD(vkGetPhysicalDeviceSurfacePresentModesKHR)
    
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
    
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(static_cast<VkPhysicalDevice> (device), win.surface, &capabilities);
    
    uint32_t num;
    vkGetPhysicalDeviceSurfaceFormatsKHR(static_cast<VkPhysicalDevice> (device), win.surface, &num, nullptr);
    formats.resize(num);
    vkGetPhysicalDeviceSurfaceFormatsKHR(static_cast<VkPhysicalDevice> (device), win.surface, &num, formats.data());
    
    vkGetPhysicalDeviceSurfacePresentModesKHR(static_cast<VkPhysicalDevice> (device), win.surface, &num, nullptr);
    presentModes.resize(num);
    vkGetPhysicalDeviceSurfacePresentModesKHR(static_cast<VkPhysicalDevice> (device), win.surface, &num, presentModes.data());
    
    VkSurfaceFormatKHR surfaceformat = chooseSwapSurfaceFormat(formats, VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(presentModes, VK_PRESENT_MODE_FIFO_KHR);
    extent = chooseSwapExtent(capabilities);
    format = surfaceformat.format;
     
    NUM_FRAMES = std::max(capabilities.minImageCount, NUM_FRAMES);
    if (capabilities.maxImageCount > 0 && NUM_FRAMES > capabilities.maxImageCount) {
        NUM_FRAMES = capabilities.maxImageCount;
    }
    
    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = win.surface;
    createInfo.minImageCount = NUM_FRAMES;
    createInfo.imageFormat = surfaceformat.format;
    createInfo.imageColorSpace = surfaceformat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 1;
    createInfo.pQueueFamilyIndices = &device.g_i;
    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = swapchain;
    
    VkSwapchainKHR newSwapchain;
    
    DEV_LOAD(vkCreateSwapchainKHR)
    
    vkCreateSwapchainKHR(device, &createInfo, nullptr, &newSwapchain);
    
    swapchain = newSwapchain;
    
    if(createInfo.oldSwapchain != VK_NULL_HANDLE) {
        
        for (auto imageView : imageViews) {
            device->destroy(imageView);
        }
        DEV_LOAD(vkDestroySwapchainKHR)
        vkDestroySwapchainKHR(device, createInfo.oldSwapchain, nullptr);
        
    }
    
    DEV_LOAD(vkGetSwapchainImagesKHR)
    
    vkGetSwapchainImagesKHR(device, swapchain, &num, nullptr);
    images.resize(num);
    vkGetSwapchainImagesKHR(device, swapchain, &num, images.data());
    
    imageViews.resize(num);
    for(uint32_t i = 0; i < num; i++) {
        
        VkImageViewCreateInfo vInfo = {};
        vInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        vInfo.image = images[i];
        vInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        vInfo.format = format;
        
        vInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        vInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        vInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        vInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        
        vInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        vInfo.subresourceRange.baseMipLevel = 0;
        vInfo.subresourceRange.levelCount = 1;
        vInfo.subresourceRange.baseArrayLayer = 0;
        vInfo.subresourceRange.layerCount = 1;
        
        imageViews[i] = device->createImageView(vk::ImageViewCreateInfo(vInfo));
        
    }
    
    DEV_LOAD(vkAcquireNextImageKHR)
    this->vkAcquireNextImageKHR = vkAcquireNextImageKHR;
    DEV_LOAD(vkQueuePresentKHR)
    this->vkQueuePresentKHR = vkQueuePresentKHR;
    
}

void Swapchain::reset() {
    
    for (auto imageView : imageViews) {
        device->destroy(imageView);
    }
    
    DEV_LOAD(vkDestroySwapchainKHR)
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    swapchain = VK_NULL_HANDLE;
    
}







/////////////
// RUNTIME //
/////////////

uint32_t Swapchain::acquire() {
    
    VkResult result;
    do {
        result = vkAcquireNextImageKHR(device, swapchain, 10000000000000L, VK_NULL_HANDLE, VK_NULL_HANDLE, &current);
        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Swapchain out of date");
        }
    } while(result != VK_SUCCESS);

    return current;
    
}

void Swapchain::present() {
    
    VkPresentInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.swapchainCount = 1;
    info.pSwapchains = &swapchain;
    info.pImageIndices = &current;
    info.pResults = nullptr;
    info.waitSemaphoreCount = 0;
    info.pWaitSemaphores = nullptr;
    
    // This will display the image
    VkResult result = vkQueuePresentKHR(static_cast<VkQueue> (device.graphics), &info);
    if(result != VK_SUCCESS) {
        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Swapchain out of date");
        }
    }
    
}






VkSurfaceFormatKHR Swapchain::chooseSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> &formats, VkFormat wantedFormat, VkColorSpaceKHR wantedColorSpace) {
    if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
        return {wantedFormat, wantedColorSpace}; // Just give the format you want
    }

    for (const auto& availableFormat : formats) {
        if (availableFormat.format == wantedFormat && availableFormat.colorSpace == wantedColorSpace) { // Look for the wanted format
            return availableFormat;
        }
    }

    return formats[0];
}

VkPresentModeKHR Swapchain::chooseSwapPresentMode(std::vector<VkPresentModeKHR> &presentModes, VkPresentModeKHR wantedMode) {

    for (const auto& availablePresentMode : presentModes) {
        if (availablePresentMode == wantedMode) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Swapchain::chooseSwapExtent(VkSurfaceCapabilitiesKHR &capabilities) {

    VkExtent2D actualExtent = {(uint32_t) win.getWidth(), (uint32_t) win.getHeight()};

    actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

    return actualExtent;

}

vk::Format Swapchain::findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) {
    for (vk::Format format : candidates) {
        vk::FormatProperties props = device.physical.getFormatProperties(format);

        if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

Swapchain::~Swapchain() {
    reset();
}
