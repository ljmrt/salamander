#ifndef DEVICEHANDLER_H
#define DEVICEHANDLER_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <optional>
#include <vector>


namespace deviceHandler
{
    // TODO: move to seperate file/directory when used more.
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;

        bool isAssigned() {
            return graphicsFamily.has_value();
        }
    };
    
    // pick a suitable graphics card for the application.
    //
    // @param vkInstance Vulkan instance to use.
    // @param resultFamilyIndices result indices of requested queue families.
    // @param resultPhysicalDevice result chosen device.
    void pickPhysicalDevice(VkInstance vkInstance, QueueFamilyIndices& resultFamilyIndices, VkPhysicalDevice& resultPhysicalDevice);

    // check if a VkPhysicalDevice is suitable for the application.
    //
    // @param device device to check.
    // @param resultFamilyIndices result indices of requested queue families.
    // @return true/false on if the device is suitable.
    bool deviceSuitable(VkPhysicalDevice device, QueueFamilyIndices& resultFamilyIndices);

    // get the supported queue families from a physical device.
    //
    // @param device device to find the queue families of.
    // @return struct with indices of the supported queue families.
    void getSupportedQueueFamilies(VkPhysicalDevice device, std::vector<VkQueueFamilyProperties>& resultQueueFamilies);

    // create a logical device using the specified queue family indices.
    //
    // @param physicalDevice physical device to use in logical device creation.
    // @param indices indices to use in logical device creation.
    // @param resultLogicalDevice created logical device.
    void createLogicalDevice(VkPhysicalDevice physicalDevice, QueueFamilyIndices indices, VkDevice& resultLogicalDevice);
};


#endif  // DEVICE_HANDLER_H
