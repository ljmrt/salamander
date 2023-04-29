#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/VulkanInstance.h>
#include <core/VulkanInstance/supportUtils.h>
#include <core/VulkanInstance/deviceHandler.h>
#include <core/DisplayManager/DisplayManager.h>
#include <core/DisplayManager/swapchainHandler.h>
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
    DisplayManager::createWindowSurface(m_vkInstance, displayDetails.glfwWindow, displayDetails.vulkanDisplayDetails.windowSurface);

    m_devices.physicalDevice = VK_NULL_HANDLE;
    deviceHandler::selectPhysicalDevice(m_vkInstance, displayDetails.vulkanDisplayDetails.windowSurface, m_familyIndices, m_devices.physicalDevice);
    
    deviceHandler::createLogicalDevice(m_devices.physicalDevice, m_familyIndices, m_devices.logicalDevice);
    vkGetDeviceQueue(m_devices.logicalDevice, m_familyIndices.graphicsFamily.value(), 0, &displayDetails.vulkanDisplayDetails.graphicsQueue);
    vkGetDeviceQueue(m_devices.logicalDevice, m_familyIndices.presentationFamily.value(), 0, &displayDetails.vulkanDisplayDetails.presentationQueue);

    swapchainHandler::createSwapchainComponentsWrapper(m_devices, displayDetails);

    swapchainHandler::createSwapchainImageViews(displayDetails.vulkanDisplayDetails.swapchainImages, displayDetails.vulkanDisplayDetails.swapchainImageFormat, m_devices.logicalDevice, displayDetails.vulkanDisplayDetails.swapchainImageViews);
}

void VulkanInstance::createVkInstance(std::string instanceApplicationName, VkInstance& createdInstance)
{
    if (supportUtils::enableValidationLayers && !supportUtils::checkValidationLayerSupport()) {
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
    supportUtils::fetchRequiredExtensions(extensions);
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (supportUtils::enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(supportUtils::requiredValidationLayers.size());
        createInfo.ppEnabledLayerNames = supportUtils::requiredValidationLayers.data();

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
    swapchainHandler::cleanupSwapchain(m_devices.logicalDevice, displayDetails.vulkanDisplayDetails.swapchainFramebuffers, displayDetails.vulkanDisplayDetails.swapchainImageViews, displayDetails.vulkanDisplayDetails.swapchain);  // clean up the swapchain and its details.
    
    vkDestroyDevice(m_devices.logicalDevice, nullptr);
    
    if (supportUtils::DEBUG_ENABLED) {
        VulkanExtensions::DestroyDebugUtilsMessengerEXT(m_vkInstance, m_debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(m_vkInstance, displayDetails.vulkanDisplayDetails.windowSurface, nullptr);
    
    vkDestroyInstance(m_vkInstance, nullptr);
}
