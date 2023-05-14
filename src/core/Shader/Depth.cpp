#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transformation.hpp>

#include <core/Shader/Depth.h>
#include <core/Shader/Image.h>
#include <core/VulkanInstance/DeviceHandler.h>
#include <core/Logging/ErrorLogger.h>


void Depth::createDepthComponents(VkExtent2D swapchainImageExtent, DeviceHandler::VulkanDevices vulkanDevices, VkCommandPool commandPool, VkQueue commandQueue, VkImage& depthImage, VkDeviceMemory& depthImageMemory, VkImageView& depthImageView)
{
    VkFormat depthImageFormat;
    Depth::selectDepthImageFormat(vulkanDevices.physicalDevice, depthImageFormat);

    Image::createImage(swapchainImageExtent.width, swapchainImageExtent.height, depthImageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vulkanDevices, depthImage, depthImageMemory);
    Image::createImageView(depthImage, depthImageFormat, VK_IMAGE_ASPECT_DEPTH_BIT, vulkanDevices.logicalDevice);

    VkCommandBuffer disposableCommandBuffer;
    CommandManager::beginRecordingSingleSubmitCommands(commandPool, vulkanDevices.logicalDevice, disposableCommandBuffer);
    
    Image::transitionImageLayout(depthImage, depthImageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, disposableCommandBuffer);
    
    CommandManager::submitSingleSubmitCommands(disposableCommandBuffer, commandPool, commandQueue, vulkanDevices.logicalDevice);
}

void Depth::selectSupportedImageFormat(const std::vector<VkFormat> candidateImageFormats, VkImageTiling imageTiling, VkFormatFeatureFlags imageFormatFeatureFlags, VkPhysicalDevice vulkanPhysicalDevice, VkFormat& imageFormat)
{
    for (VkFormat candidateImageFormat : candidateImageFormats) {
        VkFormatProperties candidateImageFormatProperties;
        vkGetPhysicalDeviceFormatProperties(vulkanPhysicalDevice, candidateImageFormat, &candidateImageFormatProperties);

        // while we could use a ||(or) and use a single if statement, it would get too complex/unreadable.
        if ((imageTiling == VK_IMAGE_TILING_LINEAR) && ((candidateImageFormatProperties.linearTilingFeatures & imageFormatFeatureFlags) == imageFormatFeatureFlags)) {
            imageFormat = candidateImageFormat;
            return;
        } else if ((imageTiling == VK_IMAGE_TILING_OPTIMAL) && ((candidateImageFormatProperties.optimalTilingFeatures & imageFormatFeatureFlags) == imageFormatFeatureFlags)) {
            imageFormat = candidateImageFormat;
            return;
        }
    }

    throwDebugException("Failed to select a supported image format.");  // no supported image format was selected.
}

void Depth::selectDepthImageFormat(VkPhysicalDevice vulkanPhysicalDevice, VkFormat& depthImageFormat)
{
    const std::vector<VkFormat> candidateDepthImageFormats = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
    Depth::selectSupportedImageFormat(candidateDepthImageFormats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, vulkanPhysicalDevice, depthImageFormat);
}

bool Depth::depthImageFormatHasStencilComponent(VkFormat depthImageFormat)
{
    return depthImageFormat == VK_FORMAT_D32_SFLOAT_S8_UINT || depthImageFormat == VK_FORMAT_D24_UNORM_S8_UINT;
}
