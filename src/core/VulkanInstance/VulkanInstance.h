#ifndef VULKANINSTANCE_H
#define VULKANINSTANCE_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>


class VulkanInstance
{
public:
    VkInstance vkInstance;
    
    VkDebugUtilsMessengerEXT debugMessenger;
    

    VulkanInstance();
    VulkanInstance(std::string instanceApplicationName);
};


#endif  // VULKAN_INSTANCE_H
