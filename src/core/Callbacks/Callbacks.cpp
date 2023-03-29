#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Callbacks/Callbacks.h>

#include <iostream>

VKAPI_ATTR VkBool32 VKAPI_CALL Callbacks::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                    void *pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}
