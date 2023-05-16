#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <core/Shader/Depth.h>
#include <core/Shader/Image.h>
#include <core/VulkanInstance/DeviceHandler.h>
#include <core/Command/CommandManager.h>
#include <core/Logging/ErrorLogger.h>


void Depth::createDepthComponents(VkExtent2D swapchainImageExtent, VkCommandPool commandPool, VkQueue commandQueue, DeviceHandler::VulkanDevices vulkanDevices, VkImage& depthImage, VkDeviceMemory& depthImageMemory, VkImageView& depthImageView)
{
    VkFormat depthImageFormat;
    Depth::selectDepthImageFormat(vulkanDevices.physicalDevice, depthImageFormat);

    Image::createImage(swapchainImageExtent.width, swapchainImageExtent.height, depthImageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vulkanDevices, depthImage, depthImageMemory);
    Image::createImageView(depthImage, depthImageFormat, VK_IMAGE_ASPECT_DEPTH_BIT, vulkanDevices.logicalDevice, depthImageView);

    VkCommandBuffer disposableCommandBuffer;
    CommandManager::beginRecordingSingleSubmitCommands(commandPool, vulkanDevices.logicalDevice, disposableCommandBuffer);
    
    Image::transitionImageLayout(depthImage, depthImageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, disposableCommandBuffer);
    
    CommandManager::submitSingleSubmitCommands(disposableCommandBuffer, commandPool, commandQueue, vulkanDevices.logicalDevice);
}

void Depth::selectDepthImageFormat(VkPhysicalDevice vulkanPhysicalDevice, VkFormat& depthImageFormat)
{
    const std::vector<VkFormat> candidateDepthImageFormats = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
    Image::selectSupportedImageFormat(candidateDepthImageFormats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, vulkanPhysicalDevice, depthImageFormat);
}

bool Depth::depthImageFormatHasStencilComponent(VkFormat depthImageFormat)
{
    return depthImageFormat == VK_FORMAT_D32_SFLOAT_S8_UINT || depthImageFormat == VK_FORMAT_D24_UNORM_S8_UINT;
}
