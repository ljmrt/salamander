#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/deviceHandler.h>
#include <core/VulkanInstance/VulkanInstance.h>
#include <core/VulkanInstance/supportUtils.h>
#include <core/Logging/ErrorLogger.h>

#include <vector>
#include <set>


void deviceHandler::pickPhysicalDevice(VulkanInstance instance, VulkanInstance::QueueFamilyIndices& resultFamilyIndices, VkPhysicalDevice& resultPhysicalDevice)
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

bool deviceHandler::deviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface, VulkanInstance::QueueFamilyIndices& resultFamilyIndices) {
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

        // if this queue family supports presentation operations.
        VkBool32 presentationSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, windowSurface, &presentationSupport);
        if (presentationSupport) {
            resultFamilyIndices.presentationFamily = i;
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

void deviceHandler::createLogicalDevice(VkPhysicalDevice physicalDevice, VulkanInstance::QueueFamilyIndices familyIndices, VkDevice& resultLogicalDevice)
{
    // create queue families specified in QueueFamilyIndices.
    std::vector<VkDeviceQueueCreateInfo> familyCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {familyIndices.graphicsFamily.value(), familyIndices.presentationFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo familyCreateInfo{};
        familyCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        familyCreateInfo.queueFamilyIndex = queueFamily;
        familyCreateInfo.queueCount = 1;
        familyCreateInfo.pQueuePriorities = &queuePriority;

        familyCreateInfos.push_back(familyCreateInfo);
    }

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
