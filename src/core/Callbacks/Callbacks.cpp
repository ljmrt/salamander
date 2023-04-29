#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Callbacks/Callbacks.h>
#include <core/Defaults/Defaults.h>

#include <iostream>
#include <ctime>


VKAPI_ATTR VkBool32 VKAPI_CALL Callbacks::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void *pUserData)
{
    time_t rawTime;
    time(&rawTime);
    struct tm *timeInfo = localtime(&rawTime);
    char buffer[80];

    strftime(buffer, 80, "%m/%d/%y %I:%M:%S", timeInfo);
    std::string timeValue(buffer);

    // set displayed type accordingly.
    std::string extendedType = "NULL";
    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
        extendedType = "GENERAL";
    } else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
        extendedType = "VALIDATION";
    } else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
        extendedType = "PERFORMANCE";
    }

    std::string colorCode;  // ANSI escape color code.
    
    // set displayed severity accordingly.
    std::string extendedSeverity = "NULL";
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        extendedSeverity = "VERBOSE";
        colorCode = "0";  // reset/normal.
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        extendedSeverity = "INFO";
        colorCode = "34";  // blue.
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        extendedSeverity = "WARNING";
        colorCode = "33";  // yellow.
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        extendedSeverity = "ERROR";
        colorCode = "31";  // red.
    }

    std::cout << "\033[" << colorCode << "m" << std::endl;  // set the appropriate color for the logging.

    // [mm/dd/yy hh:mm:ss, type:severity] message
    // ex: "[04/01/23 12:51:51, GENERAL:VERBOSE] Copying old device 0 into new device 0"
    std::cout << "[" << timeValue << ", " << extendedType << ":" << extendedSeverity << "] " << pCallbackData->pMessage << std::endl;

    std::cout << "\033[0m" << std::endl;  // reset terminal output color.

    return VK_FALSE;
}

void Callbacks::framebufferResizeCallback(GLFWwindow *window, int width, int height)
{
    Defaults::callbacksVariables.FRAMEBUFFER_RESIZED = true;
}
