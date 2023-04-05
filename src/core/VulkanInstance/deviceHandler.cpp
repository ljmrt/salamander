#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/deviceHandler.h>
#include <core/VulkanInstance/VulkanInstance.h>
#include <core/VulkanInstance/supportUtils.h>
#include <core/DisplayManager/swapChainHandler.h>
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
        if (deviceHandler::deviceSuitable(device, instance.m_displayDetails.windowSurface, resultFamilyIndices)) {
            resultPhysicalDevice = device;
            break;
        }
    }

    if (resultPhysicalDevice == VK_NULL_HANDLE) {  // resultPhysicalDevice initialized to VK_NULL_HANDLE.
        throwDebugException("failed to find a suitable GPU.");
    }
}

bool deviceHandler::deviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface, Queue::QueueFamilyIndices& resultFamilyIndices)
{
    // TODO: ranking system depending on necessary features, if the device is a dedicated graphics card, etc.
    bool extensionsSupported = deviceHandler::deviceExtensionsSuitable(physicalDevice);

    bool swapChainDetailsComplete = false;
    if (extensionsSupported) {
        swapChainHandler::SwapChainSupportDetails swapChainSupportDetails;
        swapChainHandler::querySwapChainSupportDetails(physicalDevice, windowSurface, swapChainSupportDetails);
        swapChainDetailsComplete = !swapChainSupportDetails.supportedFormats.empty() && !swapChainSupportDetails.supportedPresentationModes.empty();
    }
    
    bool queueFamiliesSupported = Queue::deviceQueueFamiliesSuitable(physicalDevice, windowSurface, resultFamilyIndices);
    
    return extensionSupported && swapChainDetailsComplete && queueFamiliesSupported;
}

bool deviceHandler::deviceExtensionsSuitable(VkPhysicalDevice physicalDevice)
{
    uint32_t supportedDeviceExtensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, supportedDeviceExtensionCount, nullptr);

    std::vector<VkExtensionProperties> supportedDeviceExtensions(supportedDeviceExtensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, supportedDeviceExtensionCount, supportedDeviceExtensions.data());

    std::set<std::string> requiredExtensions(requiredDeviceExtensions.begin(), requiredDeviceExtensions.end());
    for (VkExtensionProperties extensionProperties : supportedDeviceExtensions) {
        requiredExtensions.erase(extensionProperties.extensionName);  // remove the required extension from the required extensions copy if matching.
    }

    return requiredExtensions.empty();  // if all the required extensions were found(removed individually from required extensions list.
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
    logicalCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
    logicalCreateInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

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
