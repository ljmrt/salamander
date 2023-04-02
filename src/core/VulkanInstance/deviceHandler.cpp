#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/deviceHandler.h>
#include <core/Logging/ErrorLogger.h>

#include <vector>


void deviceHandler::pickPhysicalDevice(VkInstance vkInstance, VkPhysicalDevice& physicalDevice)
{
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, nullptr);
    if (physicalDeviceCount == 0) {
        throwDebugException("failed to find any GPUs with Vulkan support.");
    }

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, physicalDevices.data());
    for (VkPhysicalDevice device : physicalDevices) {
        if (deviceHandler::deviceSuitable(device)) {
            physicalDevice = device;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {  // physicalDevice initialized to VK_NULL_HANDLE.
        throwDebugException("failed to find a suitable GPU.");
    }
}

bool deviceHandler::deviceSuitable(VkPhysicalDevice device) {
    // TODO: ranking system depending on necessary features, if the device is a dedicated graphics card, etc.
    std::vector<VkQueueFamilyProperties> queueFamilies;
    deviceHandler::getSupportedQueueFamilies(device, queueFamilies);

    QueueFamilyIndices indices;
    int i = 0;
    for (VkQueueFamilyProperties queueFamily : queueFamilies) {
        if (indices.isAssigned()) {
            break;
        }
        
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {  // if this queue family supports graphics operations.
            indices.graphicsFamily = i;
        }
        
        i += 1;
    }
    
    return indices.isAssigned();  // check wheter the family is assigned rather than comparing to 0 as family location could be 0.
}

void deviceHandler::getSupportedQueueFamilies(VkPhysicalDevice device, std::vector<VkQueueFamilyProperties>& resultQueueFamilies)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    resultQueueFamilies = std::vector<VkQueueFamilyProperties>(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, resultQueueFamilies.data());
}
