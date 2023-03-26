#ifndef CALLBACKS_H
#define CALLBACKS_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


namespace Callbacks
{
// debug messenger callback function.
//
// handles and prints debug messages.
//
// @param messageSeverity the severity of the message(VK_DEBUG_UTILS_MESSAGE_SEVERITY_[VERBOSE|INFO|WARNING|ERROR]_BIT_EXT).
// @param messageType the type of the message(VK_DEBUG_UTILS_MESSAGE_TYPE_[GENERAL|VALIDATION|PERFORMANCE]_BIT_EXT).
// @param pCallbackData VkDebugUtilsMessengerCallbackDataEXT struct containing message details.
// @param pUserData pointer specified during callback setup.
// @return indication of causing Vulkan call abortion(always VK_FALSE for our case).
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    VkDebugUtilsMessagbeTypeFlagsEXT messageType,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                    void *pUserData);
};


#endif  // CALLBACKS_H
