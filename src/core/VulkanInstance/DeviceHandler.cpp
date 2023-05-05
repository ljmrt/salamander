#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/DeviceHandler.h>
#include <core/VulkanInstance/VulkanInstance.h>
#include <core/VulkanInstance/SupportUtils.h>
#include <core/DisplayManager/SwapchainHandler.h>
#include <core/Queue/Queue.h>
#include <core/Logging/ErrorLogger.h>

#include <vector>
#include <set>


void DeviceHandler::selectPhysicalDevice(VkInstance vkInstance, VkSurfaceKHR windowSurface, Queue::QueueFamilyIndices& queueFamilyIndices, VkPhysicalDevice& selectedPhysicalDevice)
{
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, nullptr);
    if (physicalDeviceCount == 0) {
        throwDebugException("failed to find any GPUs with Vulkan support.");
    }

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, physicalDevices.data());
    for (VkPhysicalDevice physicalDevice : physicalDevices) {
        if (DeviceHandler::deviceSuitable(physicalDevice, windowSurface, queueFamilyIndices)) {
            selectedPhysicalDevice = physicalDevice;
            break;
        }
    }

    if (selectedPhysicalDevice == VK_NULL_HANDLE) {  // resultPhysicalDevice initialized to VK_NULL_HANDLE.
        throwDebugException("failed to find a suitable GPU.");
    }
}

bool DeviceHandler::deviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface, Queue::QueueFamilyIndices& queueFamilyIndices)
{
    // TODO: ranking system depending on necessary features, if the device is a dedicated graphics card, etc.
    bool extensionsSupported = DeviceHandler::deviceExtensionsSuitable(physicalDevice);

    bool swapchainDetailsComplete = false;
    if (extensionsSupported) {
        SwapchainHandler::SwapchainSupportDetails swapchainSupportDetails;
        SwapchainHandler::querySwapchainSupportDetails(physicalDevice, windowSurface, swapchainSupportDetails);
        swapchainDetailsComplete = !swapchainSupportDetails.supportedSurfaceFormats.empty() && !swapchainSupportDetails.supportedPresentationModes.empty();
    }
    
    bool queueFamiliesSupported = Queue::deviceQueueFamiliesSuitable(physicalDevice, windowSurface, queueFamilyIndices);
    
    return extensionsSupported && swapchainDetailsComplete && queueFamiliesSupported;
}

bool DeviceHandler::deviceExtensionsSuitable(VkPhysicalDevice physicalDevice)
{
    uint32_t supportedDeviceExtensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &supportedDeviceExtensionCount, nullptr);

    std::vector<VkExtensionProperties> supportedDeviceExtensions(supportedDeviceExtensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &supportedDeviceExtensionCount, supportedDeviceExtensions.data());

    std::set<std::string> requiredExtensions(requiredDeviceExtensions.begin(), requiredDeviceExtensions.end());
    for (VkExtensionProperties extensionProperties : supportedDeviceExtensions) {
        requiredExtensions.erase(extensionProperties.extensionName);  // remove the required extension from the required extensions copy if matching.
    }

    return requiredExtensions.empty();  // if all the required extensions were found(removed individually from required extensions list.
}

void DeviceHandler::createLogicalDevice(VkPhysicalDevice physicalDevice, Queue::QueueFamilyIndices queueFamilyIndices, VkDevice& createdLogicalDevice)
{
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    Queue::populateQueueCreateInfos(queueFamilyIndices, queueCreateInfos);
    
    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo logicalCreateInfo{};
    logicalCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    logicalCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    logicalCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    logicalCreateInfo.pEnabledFeatures = &deviceFeatures;
    logicalCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
    logicalCreateInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

    if (SupportUtils::enableValidationLayers) {
        logicalCreateInfo.enabledLayerCount = static_cast<uint32_t>(SupportUtils::requiredValidationLayers.size());
        logicalCreateInfo.ppEnabledLayerNames = SupportUtils::requiredValidationLayers.data();
    } else {
        logicalCreateInfo.enabledLayerCount = 0;
    }

    int creationResult = vkCreateDevice(physicalDevice, &logicalCreateInfo, nullptr, &createdLogicalDevice);
    if (creationResult != VK_SUCCESS) {
        throwDebugException("Failed to create logical device.");
    }
}
