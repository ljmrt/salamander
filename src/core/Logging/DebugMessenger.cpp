#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Logging/DebugMessenger.h>
#include <core/VulkanInstance/VulkanInstance.h>
#include <core/VulkanInstance/supportUtils.h>
#include <core/Logging/ErrorLogger.h>
#include <core/VulkanExtensions/VulkanExtensions.h>
#include <core/Callbacks/Callbacks.h>


void DebugMessenger::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& resultCreateInfo)
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
    resultCreateInfo.pUserData = nullptr;
}

void DebugMessenger::createDebugMessenger(VkInstance vkInstance, VkDebugUtilsMessengerEXT& resultDebugMessenger)
{
    if (supportUtils::DEBUG_ENABLED == false) {
        return;
    }
    
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    DebugMessenger::populateDebugMessengerCreateInfo(createInfo);
    
    int creationResult = VulkanExtensions::CreateDebugUtilsMessengerEXT(vkInstance, &createInfo, nullptr, &resultDebugMessenger);
    if (creationResult != VK_SUCCESS) {
        throwDebugException("Failed to set up debug messenger.");
    }
}
