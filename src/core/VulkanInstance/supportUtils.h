#ifndef SUPPORTUTILS_H
#define SUPPORTUTILS_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>


namespace supportUtils
{
    // TODO: move elsewhere.
    #ifdef NDEBUG
        const bool DEBUG_ENABLED = false;
    #else
        const bool DEBUG_ENABLED = true;
    #endif
    
    const std::vector<const char *> m_validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    const bool m_enableValidationLayers = DEBUG_ENABLED;
    

    // get the required extensions(from GLFW and others).2
    //
    // @return vector containing extension names.
    std::vector<const char *> getRequiredExtensions();
    
    // checks if all the required validation layers are supported.
    //
    // @return validation layer support.
    bool checkValidationLayerSupport();
}


#endif  // SUPPORTUTILS_H
