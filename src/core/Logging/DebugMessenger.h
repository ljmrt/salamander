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
    // @param resultCreateInfo stored create info from function.
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& resultCreateInfo);
    
    // create a debug messenger and store it in resultDebugMessenger.
    //
    // @param vkInstance Vulkan Instance passed to the function to properly create the messenger.
    // @param resultDebugMessenger stored messenger from function.
    void createDebugMessenger(VkInstance vkInstance, VkDebugUtilsMessengerEXT& resultDebugMessenger);
};


#endif  // DEBUGMESSENGER_H
