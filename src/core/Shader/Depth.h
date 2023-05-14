#ifndef DEPTH_H
#define DEPTH_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


namespace Depth
{
    // TODO: function documentation.
    void createDepthComponents(VkExtent2D swapchainImageExtent, DeviceHandler::VulkanDevices vulkanDevices, VkCommandPool commandPool, VkQueue commandQueue, VkImage& depthImage, VkDeviceMemory& depthImageMemory, VkImageView& depthImageView);

    void selectSupportedImageFormat(const std::vector<VkFormat> candidateImageFormats, VkImageTiling imageTiling, VkFormatFeatureFlags imageFormatFeatureFlags, VkPhysicalDevice vulkanPhysicalDevice, VkFormat& imageFormat);

    void selectDepthImageFormat(VkPhysicalDevice vulkanPhysicalDevice, VkFormat& depthImageFormat);

    bool depthImageFormatHasStencilComponent(VkFormat depthImageFormat);
}


#endif  // DEPTH_H
