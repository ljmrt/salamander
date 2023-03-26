#ifndef VULKANINSTANCE_H
#define VULKANINSTANCE_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


class VulkanInstance
{
public:
    VkInstance m_instance;
    
    VkDebugUtilsMessengerEXT m_debugMessenger;
    

    VulkanInstance(const char *instanceApplicationName);
};


#endif  // VULKAN_INSTANCE_H
