#ifndef INSTANCE_H
#define INSTANCE_H

#include <vulkan/vulkan.hpp>

class Windu;

class Instance {
public:
    Instance(Windu& win);
    ~Instance();
    vk::Instance* operator->() {return &instance;}
    operator vk::Instance() { return instance; }
    operator VkInstance() { return static_cast<VkInstance>(instance); }
    bool supportsPresent(VkPhysicalDevice device, int i);
    vk::Instance instance;
    VkDebugUtilsMessengerEXT messenger;
private:
    Windu& win;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;
};

#endif
