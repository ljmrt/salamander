#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/deviceHandler.h>
#include <core/VulkanInstance/supportUtils.h>
#include <core/Logging/ErrorLogger.h>

#include <vector>


void deviceHandler::pickPhysicalDevice(VkInstance vkInstance, QueueFamilyIndices& resultFamilyIndices, VkPhysicalDevice& resultPhysicalDevice)
{
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, nullptr);
    if (physicalDeviceCount == 0) {
        throwDebugException("failed to find any GPUs with Vulkan support.");
    }

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, physicalDevices.data());
    for (VkPhysicalDevice device : physicalDevices) {
        if (deviceHandler::deviceSuitable(device, resultFamilyIndices)) {
            resultPhysicalDevice = device;
            break;
        }
    }

    if (resultPhysicalDevice == VK_NULL_HANDLE) {  // resultPhysicalDevice initialized to VK_NULL_HANDLE.
        throwDebugException("failed to find a suitable GPU.");
    }
}

bool deviceHandler::deviceSuitable(VkPhysicalDevice physicalDevice, QueueFamilyIndices& resultFamilyIndices) {
    // TODO: ranking system depending on necessary features, if the device is a dedicated graphics card, etc.
    std::vector<VkQueueFamilyProperties> queueFamilies;
    deviceHandler::getSupportedQueueFamilies(physicalDevice, queueFamilies);

    int i = 0;
    for (VkQueueFamilyProperties queueFamily : queueFamilies) {
        if (resultFamilyIndices.isAssigned()) {
            break;
        }
        
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {  // if this queue family supports graphics operations.
            resultFamilyIndices.graphicsFamily = i;
        }
        
        i += 1;
    }
    
    return resultFamilyIndices.isAssigned();  // check whether the family is assigned rather than comparing to 0 as family location could be 0.
}

void deviceHandler::getSupportedQueueFamilies(VkPhysicalDevice device, std::vector<VkQueueFamilyProperties>& resultQueueFamilies)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    resultQueueFamilies = std::vector<VkQueueFamilyProperties>(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, resultQueueFamilies.data());
}

void deviceHandler::createLogicalDevice(VkPhysicalDevice physicalDevice, QueueFamilyIndices familyIndices, VkDevice& resultLogicalDevice)
{
    // create a queue in the graphics queue famiy.
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = familyIndices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;
    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo logicalCreateInfo{};
    logicalCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    logicalCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    logicalCreateInfo.queueCreateInfoCount = 1;
    logicalCreateInfo.pEnabledFeatures = &deviceFeatures;
    logicalCreateInfo.enabledExtensionCount = 0;

    if (supportUtils::m_enableValidationLayers) {
        logicalCreateInfo.enabledLayerCount = static_cast<uint32_t>(supportUtils::m_validationLayers.size());
        logicalCreateInfo.ppEnabledLayerNames = supportUtils::m_validationLayers.data();
    } else {
        logicalCreateInfo.enabledLayerCount = 0;
    }

    int creationResult = vkCreateDevice(physicalDevice, &logicalCreateInfo, nullptr, &resultLogicalDevice);
    if (creationResult != VK_SUCCESS) {
        throwDebugException("Failed to create logical device.");
    }
}
