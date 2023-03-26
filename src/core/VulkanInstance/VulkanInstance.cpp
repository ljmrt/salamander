#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/VulkanInstance.h>
#include <core/VulkanInstance/supportUtils.h>

#include <cstdint>
#include <iostream>


VulkanInstance::VulkanInstance(const char *instanceApplicationName)
{
    if (supportUtils::enableValidationLayers && !supportUtils::checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested but not availible!");  // TODO: runtime error helper with line and file number.
    }
    
    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = instanceApplicationName;
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName = "No Engine";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &applicationInfo;

    std::vector<const char *> extensions = supportUtils::getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (supportUtils::enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(supportUtils::validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    VkResult instanceResult = vkCreateInstance(&createInfo, nullptr, &m_instance);
    if (instanceResult != VK_SUCCESS) {
        throw std::runtime_error("failed to create vulkan instance!");
    }
}
