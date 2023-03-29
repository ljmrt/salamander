#ifndef EXTENSIONS_H
#define EXTENSIONS_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace VulkanExtensions
{
    // drop-in replacement to load the DebugUtilsMessenger version of this function.
    //
    // necessary due to the lack of loading due to the extension nature.
    //
    // @param see Vulkan specification/documentation.
    // @return see Vulkan specification/documentation.
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

    // drop-in replacement to load the DebugUtilsMessenger version of this function.
    //
    // necessary due to the lack of loading due to the extension nature.
    //
    // @param see Vulkan specification/documentation.
    // @return see Vulkan specification/documentation.
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
}


#endif  // EXTENSIONS_H
