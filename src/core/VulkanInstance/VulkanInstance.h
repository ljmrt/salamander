#ifndef VULKANINSTANCE_H
#define VULKANINSTANCE_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/DisplayManager/DisplayManager.h>
#include <core/Queue/Queue.h>

#include <string>


class VulkanInstance
{
private:
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;  // selected physical device
    
    
    void createVkInstance(std::string instanceApplicationName, VkInstance& resultInstance);
public:
    VkInstance m_vkInstance;  // actual Vulkan instance.
    
    VkDebugUtilsMessengerEXT m_debugMessenger;  // debug/logging messenger.
    
    DisplayManager::DisplayDetails m_displayDetails;  // glfw window, window surface, swap chain, swap chain images, etc.

    Queue::QueueFamilyIndices m_familyIndices;
    VkQueue m_graphicsQueue;
    VkQueue m_presentationQueue;
    
    VkDevice m_logicalDevice;  // created logical device.
    

    VulkanInstance();
    VulkanInstance(std::string instanceApplicationName, GLFWwindow *rendererWindow);

    void cleanupInstance();
};


#endif  // VULKAN_INSTANCE_H
