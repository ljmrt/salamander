#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/deviceHandler.h>
#include <core/VulkanInstance/VulkanInstance.h>
#include <core/VulkanInstance/supportUtils.h>
#include <core/Queue/Queue.h>
#include <core/Logging/ErrorLogger.h>

#include <vector>
#include <set>


void deviceHandler::pickPhysicalDevice(VulkanInstance instance, Queue::QueueFamilyIndices& resultFamilyIndices, VkPhysicalDevice& resultPhysicalDevice)
{
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance.m_vkInstance, &physicalDeviceCount, nullptr);
    if (physicalDeviceCount == 0) {
        throwDebugException("failed to find any GPUs with Vulkan support.");
    }

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance.m_vkInstance, &physicalDeviceCount, physicalDevices.data());
    for (VkPhysicalDevice device : physicalDevices) {
        if (deviceHandler::deviceSuitable(device, instance.m_windowSurface, resultFamilyIndices)) {
            resultPhysicalDevice = device;
            break;
        }
    }

    if (resultPhysicalDevice == VK_NULL_HANDLE) {  // resultPhysicalDevice initialized to VK_NULL_HANDLE.
        throwDebugException("failed to find a suitable GPU.");
    }
}

bool deviceHandler::deviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface, Queue::QueueFamilyIndices& resultFamilyIndices) {
    // TODO: ranking system depending on necessary features, if the device is a dedicated graphics card, etc.
    return Queue::deviceQueueFamiliesSuitable(physicalDevice, windowSurface, resultFamilyIndices);  // only consideration currently is if all required queue families are supported.
}

void deviceHandler::createLogicalDevice(VkPhysicalDevice physicalDevice, Queue::QueueFamilyIndices familyIndices, VkDevice& resultLogicalDevice)
{
    std::vector<VkDeviceQueueCreateInfo> familyCreateInfos;
    Queue::getFamilyCreateInfos(familyIndices, familyCreateInfos);
    
    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo logicalCreateInfo{};
    logicalCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    logicalCreateInfo.pQueueCreateInfos = familyCreateInfos.data();
    logicalCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(familyCreateInfos.size());
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
