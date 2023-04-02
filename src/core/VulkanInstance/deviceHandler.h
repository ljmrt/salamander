#ifndef DEVICEHANDLER_H
#define DEVICEHANDELR_H


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
    // @param resultPhysicalDevice result chosen device.
    void pickPhysicalDevice(VkInstance vkInstance, VkPhysicalDevice& resultPhysicalDevice);

    // check if a VkPhysicalDevice is suitable for the application.
    //
    // @param device device to check.
    // @return true/false on if the device is suitable.
    bool deviceSuitable(VkPhysicalDevice device);

    // get the supported queue families from a physical device.
    //
    // @param device device to find the queue families of.
    // @return struct with indices of the supported queue families.
    void getSupportedQueueFamilies(VkPhysicalDevice device, std::vector<VkQueueFamilyProperties>& resultQueueFamilies);
};


#endif  // DEVICE_HANDLER_H
