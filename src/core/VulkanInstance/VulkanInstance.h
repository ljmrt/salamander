#ifndef VULKANINSTANCE_H
#define VULKANINSTANCE_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/deviceHandler.h>

#include <string>


class VulkanInstance
{
private:
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    
    
    void createVkInstance(std::string instanceApplicationName, VkInstance& resultInstance);
public:
    VkInstance m_vkInstance;
    
    VkDebugUtilsMessengerEXT m_debugMessenger;

    deviceHandler::QueueFamilyIndices m_familyIndices;
    VkQueue m_graphicsQueue;
    VkDevice m_logicalDevice;
    

    VulkanInstance();
    VulkanInstance(std::string instanceApplicationName);
};


#endif  // VULKAN_INSTANCE_H
