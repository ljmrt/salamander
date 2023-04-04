#ifndef DEVICEHANDLER_H
#define DEVICEHANDLER_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/VulkanInstance.h>
#include <core/Queue/Queue.h>

#include <vector>


namespace deviceHandler
{    
    // pick a suitable graphics card for the application.
    //
    // @param instance Vulkan instance **class** to use in selection.
    // @param resultFamilyIndices result indices of requested queue families.
    // @param resultPhysicalDevice result chosen device.
    void pickPhysicalDevice(VulkanInstance instance, Queue::QueueFamilyIndices& resultFamilyIndices, VkPhysicalDevice& resultPhysicalDevice);

    // check if a VkPhysicalDevice is suitable for the application.
    //
    // @param device device to check.
    // @param windowSurface window surface to use in suitability checks.
    // @param resultFamilyIndices result indices of requested queue families.
    // @return true/false on if the device is suitable.
    bool deviceSuitable(VkPhysicalDevice device, VkSurfaceKHR windowSurface, Queue::QueueFamilyIndices& resultFamilyIndices);

    // create a logical device using the specified queue family indices.
    //
    // @param physicalDevice physical device to use in logical device creation.
    // @param indices indices to use in logical device creation.
    // @param resultLogicalDevice created logical device.
    void createLogicalDevice(VkPhysicalDevice physicalDevice, Queue::QueueFamilyIndices indices, VkDevice& resultLogicalDevice);
};


#endif  // DEVICE_HANDLER_H
