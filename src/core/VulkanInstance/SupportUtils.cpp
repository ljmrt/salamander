#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/SupportUtils.h>

#include <vector>
#include <algorithm>
#include <iterator>
#include <string>


void SupportUtils::fetchRequiredExtensions(std::vector<const char *>& requiredExtensions)
{
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (DEBUG_ENABLED) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    requiredExtensions = extensions;
}

bool SupportUtils::checkValidationLayerSupport()
{
    uint32_t supportedLayerCount;
    vkEnumerateInstanceLayerProperties(&supportedLayerCount, nullptr);

    std::vector<VkLayerProperties> supportedLayers(supportedLayerCount);
    vkEnumerateInstanceLayerProperties(&supportedLayerCount, supportedLayers.data());

    std::vector<std::string> supportedLayerNames;
    supportedLayerNames.reserve(supportedLayerCount);
    // strip the layer data from the supported layers, only get layer names in a new vector.
    for (VkLayerProperties supportedLayer : supportedLayers) {
        supportedLayerNames.push_back(supportedLayer.layerName);
    }

    for (const char *validationLayerName : requiredValidationLayers) {
        if(std::find(supportedLayerNames.begin(), supportedLayerNames.end(), validationLayerName) == supportedLayerNames.end()) {  // if required validation layer name not found at all in the supported layers.
            return false;
        }
    }
    return true;
}
