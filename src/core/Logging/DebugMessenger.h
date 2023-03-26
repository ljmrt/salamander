#ifndef DEBUGMESSENGER_H
#define DEBUGMESSENGER_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/VulkanInstance.h>


namespace DebugMessenger
{
private:
    // TODO: move elsewhere.
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
    
public:
    // populate a debug messenger's create info.
    //
    // hardcoded filters.
    //
    // @param resultCreateInfo stored create info from function.
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& resultCreateInfo);
    
    // create a debug messenger and store it in resultDebugMessenger.
    //
    // @param resultDebugMessenger stored messenger from function.
    void createDebugMessenger(VulkanInstance vulkanInstance, VkDebugUtilsMessengerEXT& resultDebugMessenger);
};


#endif  // DEBUGMESSENGER_H
