#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <vulkan/vulkan.hpp>

class Windu;
class Instance;
class Device;

class Swapchain {
public :
    Swapchain(Windu& win, Instance& instance, Device &device);
    void init();
    ~Swapchain();
    void reset();
    
    uint32_t acquire(vk::Semaphore signal);
    void present(vk::Semaphore wait);
    
    Windu& win;
    Instance& instance;
    Device& device;
    
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    VkFormat format;
    VkExtent2D extent;
    
    vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);

    uint32_t NUM_FRAMES = 3;
    uint32_t current = 1000;
    
private :
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> &formats, VkFormat wantedFormat, VkColorSpaceKHR wantedColorSpace);
    VkPresentModeKHR chooseSwapPresentMode(std::vector<VkPresentModeKHR> &presentModes, VkPresentModeKHR wantedMode);
    VkExtent2D chooseSwapExtent(VkSurfaceCapabilitiesKHR &capabilities);
    
    PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
    PFN_vkQueuePresentKHR vkQueuePresentKHR;
    
    uint64_t last = 0;
    double frametime = 0.0, count = 0.0;
};

#endif

