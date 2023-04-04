#ifndef VULKANINSTANCE_H
#define VULKANINSTANCE_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <optional>


class VulkanInstance
{
private:
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;  // selected physical device
    
    
    void createVkInstance(std::string instanceApplicationName, VkInstance& resultInstance);
public:
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentationFamily;

        bool isAssigned() {
            return graphicsFamily.has_value() && presentationFamily.has_value();
        }
    };
    
    VkInstance m_vkInstance;  // actual Vulkan instance.
    
    VkDebugUtilsMessengerEXT m_debugMessenger;  // debug/logging messenger.
    
    VkSurfaceKHR m_windowSurface;  // window/presentation surface.

    QueueFamilyIndices m_familyIndices;
    VkQueue m_graphicsQueue;
    VkQueue m_presentationQueue;
    
    VkDevice m_logicalDevice;  // created logical device.
    

    VulkanInstance();
    VulkanInstance(std::string instanceApplicationName, GLFWwindow *rendererWindow);

    void cleanupInstance();
};


#endif  // VULKAN_INSTANCE_H
