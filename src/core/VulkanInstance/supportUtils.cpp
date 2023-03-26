#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VKInstance/supportUtils.h>

#include <vector>
#include <cstring>


std::vector<const char *> getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (DEBUG_ENABLED) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
}

bool supportUtils::checkValidationlayerSupport()
{
    uint32_t supportedLayerCount;
    vkEnumerateInstanceLayerProperties(&supportedLayerCount, nullptr);

    // get the availible layers from the supported layer count.
    std::vector<VkLayerProperties> supportedLayers(supportedLayerCount);
    vkEnumerateInstanceLayerProperties(&supportedLayerCount, supportedLayers.data());

    for (const char *validationLayerName : m_validationLayers) {
        for (const VkLayerProperties& supportedLayer& : supportedLayers) {
            if (strcmp(validationlayerName, supportedLayer.layerName) != 0) {  // if layer names are not equal.
                return false;
            }
        }
    }
    return true;
}
