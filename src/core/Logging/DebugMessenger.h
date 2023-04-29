#ifndef DEBUGMESSENGER_H
#define DEBUGMESSENGER_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/VulkanInstance.h>
#include <core/Config/Config.h>


namespace DebugMessenger
{   
    // populate a debug messenger's create info.
    //
    // configurable filters.
    //
    // @param populatedCreateInfo stored populated debug messenger create info.
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& populatedCreateInfo);
    
    // create a debug messenger.
    //
    // @param vkInstance Vulkan instance used in debug messenger creation.
    // @param createdDebugMessenger created debug messenger.
    void createDebugMessenger(VkInstance vkInstance, VkDebugUtilsMessengerEXT& createdDebugMessenger);
};


#endif  // DEBUGMESSENGER_H
