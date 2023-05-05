#ifndef DEVICEHANDLER_H
#define DEVICEHANDLER_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Queue/Queue.h>

#include <vector>


namespace DeviceHandler
{
    struct VulkanDevices {
        VkPhysicalDevice physicalDevice;
        VkDevice logicalDevice;
    };
    
    namespace
    {
        const std::vector<const char *> requiredDeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
    }

    
    // select a suitable graphics card for the application.
    //
    // @param vkInstance Vulkan instance to use in device selection.
    // @param windowSurface Vulkan window surface to use in selection.
    // @param queueFamilyIndices result indices of requested queue families.
    // @param selectedPhysicalDevice selected physical device.
    void selectPhysicalDevice(VkInstance vkInstance, VkSurfaceKHR windowSurface, Queue::QueueFamilyIndices& queueFamilyIndices, VkPhysicalDevice& selectedPhysicalDevice);

    // check if a VkPhysicalDevice is suitable for the application.
    //
    // @param physicalDevice physicalDevice to check suitability of.
    // @param windowSurface window surface to use in suitability checks.
    // @param queueFamilyIndices result indices of requested queue families.
    // @return true/false on if the device is suitable.
    bool deviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface, Queue::QueueFamilyIndices& queueFamilyIndices);

    // check if a physical device supports the required extensions.
    //
    // @param physicalDevice physical device to check compatability of.
    // @return suitabilty of device.
    bool deviceExtensionsSuitable(VkPhysicalDevice physicalDevice);

    // create a logical device using the specified queue family indices.
    //
    // @param physicalDevice physical device to use in logical device creation.
    // @param queueFamilyIndices queue family indices to use in logical device creation.
    // @param createdLogicalDevice created logical device.
    void createLogicalDevice(VkPhysicalDevice physicalDevice, Queue::QueueFamilyIndices queueFamilyIndices, VkDevice& createdLogicalDevice);
};


#endif  // DEVICEHANDLER_H
