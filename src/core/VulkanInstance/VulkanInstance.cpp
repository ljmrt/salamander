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
    deviceHandler::pickPhysicalDevice(*this, displayDetails.vulkanDisplayDetails.windowSurface, m_familyIndices, m_physicalDevice);
    
    deviceHandler::createLogicalDevice(m_physicalDevice, m_familyIndices, m_logicalDevice);
    vkGetDeviceQueue(m_logicalDevice, m_familyIndices.graphicsFamily.value(), 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_logicalDevice, m_familyIndices.presentationFamily.value(), 0, &m_presentationQueue);

    swapchainHandler::createSwapchain(m_physicalDevice,
                                      m_logicalDevice,
                                      displayDetails.glfwWindow,
                                      displayDetails.vulkanDisplayDetails.windowSurface,
                                      displayDetails.vulkanDisplayDetails.swapchain,
                                      displayDetails.vulkanDisplayDetails.swapchainImages,
                                      displayDetails.vulkanDisplayDetails.swapchainImageFormat,
                                      displayDetails.vulkanDisplayDetails.swapchainExtent);

    DisplayManager::createImageViews(displayDetails.vulkanDisplayDetails, m_logicalDevice);
}

void VulkanInstance::createVkInstance(std::string instanceApplicationName, VkInstance& resultInstance)
{
    if (supportUtils::m_enableValidationLayers && !supportUtils::checkValidationLayerSupport()) {
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

    std::vector<const char *> extensions = supportUtils::getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (supportUtils::m_enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(supportUtils::m_validationLayers.size());
        createInfo.ppEnabledLayerNames = supportUtils::m_validationLayers.data();

        DebugMessenger::populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    VkResult instanceResult = vkCreateInstance(&createInfo, nullptr, &resultInstance);
    if (instanceResult != VK_SUCCESS) {
        throwDebugException("Failed to create vulkan instance.");
    }
}

void VulkanInstance::cleanupInstance(DisplayManager::DisplayDetails displayDetails)
{
    for (VkImageView imageView : displayDetails.vulkanDisplayDetails.swapchainImageViews) {
        vkDestroyImageView(m_logicalDevice, imageView, nullptr);
    }
    
    vkDestroySwapchainKHR(m_logicalDevice, displayDetails.vulkanDisplayDetails.swapchain, nullptr);
    
    vkDestroyDevice(m_logicalDevice, nullptr);
    
    if (supportUtils::DEBUG_ENABLED) {
        VulkanExtensions::DestroyDebugUtilsMessengerEXT(m_vkInstance, m_debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(m_vkInstance, displayDetails.vulkanDisplayDetails.windowSurface, nullptr);
    
    vkDestroyInstance(m_vkInstance, nullptr);
}
