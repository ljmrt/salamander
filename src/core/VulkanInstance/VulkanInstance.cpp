#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/VulkanInstance.h>
#include <core/VulkanInstance/SupportUtils.h>
#include <core/VulkanInstance/DeviceHandler.h>
#include <core/DisplayManager/DisplayManager.h>
#include <core/DisplayManager/SwapchainHandler.h>
#include <core/Logging/ErrorLogger.h>
#include <core/Logging/DebugMessenger.h>
#include <core/VulkanExtensions/VulkanExtensions.h>

#include <cstdint>
#include <iostream>
#include <string>


VulkanInstance::VulkanInstance()
{
    
}

VulkanInstance::VulkanInstance(std::string instanceApplicationName, DisplayManager::DisplayDetails& displayDetails)
{
    this->createVkInstance(instanceApplicationName, m_vkInstance);
        
    DebugMessenger::createDebugMessenger(m_vkInstance, m_debugMessenger);
    DisplayManager::createWindowSurface(m_vkInstance, displayDetails.glfwWindow, displayDetails.windowSurface);

    m_devices.physicalDevice = VK_NULL_HANDLE;
    DeviceHandler::selectPhysicalDevice(m_vkInstance, displayDetails.windowSurface, m_familyIndices, m_devices.physicalDevice, displayDetails.msaaSampleCount);
    
    DeviceHandler::createLogicalDevice(m_devices.physicalDevice, m_familyIndices, m_devices.logicalDevice);
    vkGetDeviceQueue(m_devices.logicalDevice, m_familyIndices.graphicsFamily.value(), 0, &displayDetails.graphicsQueue);
    vkGetDeviceQueue(m_devices.logicalDevice, m_familyIndices.presentationFamily.value(), 0, &displayDetails.presentationQueue);

    SwapchainHandler::createSwapchainComponentsWrapper(m_devices, displayDetails);

    SwapchainHandler::createSwapchainImageViews(displayDetails.swapchainImages, displayDetails.swapchainImageFormat, m_devices.logicalDevice, displayDetails.swapchainImageViews);
}

void VulkanInstance::createVkInstance(std::string instanceApplicationName, VkInstance& createdInstance)
{
    if (SupportUtils::enableValidationLayers && !SupportUtils::checkValidationLayerSupport()) {
        throwDebugException("Validation layers requested but not availible.");
    }
    
    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    
    applicationInfo.pApplicationName = instanceApplicationName.c_str();
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName = "No Engine";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &applicationInfo;

    std::vector<const char *> extensions;
    SupportUtils::fetchRequiredExtensions(extensions);
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (SupportUtils::enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(SupportUtils::requiredValidationLayers.size());
        createInfo.ppEnabledLayerNames = SupportUtils::requiredValidationLayers.data();

        DebugMessenger::populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    VkResult instanceResult = vkCreateInstance(&createInfo, nullptr, &createdInstance);
    if (instanceResult != VK_SUCCESS) {
        throwDebugException("Failed to create vulkan instance.");
    }
}

void VulkanInstance::cleanupInstance(DisplayManager::DisplayDetails displayDetails)
{
    displayDetails.cleanupDisplayDetails(m_devices.logicalDevice, false);
    
    vkDestroyDevice(m_devices.logicalDevice, nullptr);
    
    if (SupportUtils::DEBUG_ENABLED) {
        VulkanExtensions::DestroyDebugUtilsMessengerEXT(m_vkInstance, m_debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(m_vkInstance, displayDetails.windowSurface, nullptr);
    
    vkDestroyInstance(m_vkInstance, nullptr);
}
