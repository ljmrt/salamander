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

    // int verboseEnabled = std::stoi(m_loggingConfig.lookupKey("SEVERITY_VERBOSE"));
    // int infoEnabled = std::stoi(m_loggingConfig.lookupKey("SEVERITY_INFO"));
    // int warningEnabled = std::stoi(m_loggingConfig.lookupKey("SEVERITY_WARNING"));
    // int errorEnabled = std::stoi(m_loggingConfig.lookupKey("SEVERITY_ERROR"));
    resultCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT // * verboseEnabled
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT // * infoEnabled
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT // * warningEnabled
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT; // * errorEnabled;

    // int generalEnabled = std::stoi(m_loggingConfig.lookupKey("TYPE_GENERAL"));
    // int validationEnabled = std::stoi(m_loggingConfig.lookupKey("TYPE_VALIDATION"));
    // int performanceEnabled = std::stoi(m_loggingConfig.lookupKey("TYPE_PERFORMANCE"));
    resultCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT // * generalEnabled
        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT // * validationEnabled
        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT; // * performanceEnabled;

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
