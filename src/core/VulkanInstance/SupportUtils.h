#ifndef SUPPORTUTILS_H
#define SUPPORTUTILS_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>


namespace SupportUtils
{
    #ifdef NDEBUG
        const bool DEBUG_ENABLED = false;
    #else
        const bool DEBUG_ENABLED = true;
    #endif
    
    const std::vector<const char *> requiredValidationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    const bool enableValidationLayers = DEBUG_ENABLED;
    

    // fetch the required extensions(those needed by GLFW, etc.).
    //
    // @param requiredExtensions fetched required extensions.
    void fetchRequiredExtensions(std::vector<const char *>& requiredExtensions);
    
    // checks if all the required validation layers are supported.
    //
    // @return validation layer support.
    bool checkValidationLayerSupport();
}


#endif  // SUPPORTUTILS_H
