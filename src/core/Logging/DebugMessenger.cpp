#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Logging/DebugMessenger.h>
#include <core/VulkanInstance/VulkanInstance.h>
#include <core/Logging/ErrorLogger.h>


VkResult DebugMessenger::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto loadedFunction = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");  // lookup vkCreateDebugUtilsMessengerEXT function.

    if (loadedFunction != nullptr) {
        return loadedFunction(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DebugMessenger::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto loadedFunction = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

    if (loadedFunction != nullptr) {
        loadedFunction(instance, debugMessenger, pAllocator);
    }
}

void DebugMessenger::populateDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT& resultCreateInfo)
{
    resultCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    
    resultCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    
    resultCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    resultCreateInfo.pfnUserCallback = Callbacks::debugCallback;
    createInfo.pUserData = nullptr;
}

void DebugMessender::createDebugMessenger(VulkanInstance vulkanInstance, VkDebugUtilsMessengerEXT& resultDebugMessenger)
{
    if (supportUtils::DEBUG_ENABLED == FALSE) {
        return;
    }
    
    VkDebugUtilsMessengerCreateInfo createInfo;
    populateDebugMessenger(createInfo);
    
    int creationResult = CreateDebugUtilsMessengerEXT(vulkanInstance.m_instance, &createInfo, nullptr, &resultDebugMessenger);
    if (creationResult != VK_SUCCESS) {
        ErrorLogger::throwDebugException("Failed to set up debug messenger.");
    }
}
