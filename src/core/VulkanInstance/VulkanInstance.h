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
    // create a Vulkan instance using the supplied parameters.
    //
    // @param instanceApplicationName application name to use in the Vulkan instance.
    // @param resultInstance stored resulting Vulkan instance.
    void createVkInstance(std::string instanceApplicationName, VkInstance& resultInstance);
public:
    VkInstance m_vkInstance;  // actual Vulkan instance.
    
    VkDebugUtilsMessengerEXT m_debugMessenger;  // debug/logging messenger.
    
    Queue::QueueFamilyIndices m_familyIndices;

    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;  // selected physical device
    VkDevice m_logicalDevice;  // created logical device.


    // clean up this class/instance.
    //
    // @param displayDetails displayDetails.to use in clean up.
    // @param swapchainFramebuffers swapchain framebuffers to clean up.
    void cleanupInstance(DisplayManager::DisplayDetails displayDetails, std::vector<VkFramebuffer> swapchainFramebuffers);

    VulkanInstance();
    VulkanInstance(std::string instanceApplicationName, DisplayManager::DisplayDetails& displayDetails);
};


#endif  // VULKAN_INSTANCE_H
