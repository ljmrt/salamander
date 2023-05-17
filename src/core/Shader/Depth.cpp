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


void Depth::createDepthComponents(VkExtent2D swapchainImageExtent, VkCommandPool commandPool, VkQueue commandQueue, DeviceHandler::VulkanDevices vulkanDevices, Image::Image& depthImage)
{
    Depth::selectDepthImageFormat(vulkanDevices.physicalDevice, depthImage.imageFormat);
    Image::createImage(swapchainImageExtent.width, swapchainImageExtent.height, depthImage.imageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vulkanDevices, depthImage);
    Image::createImageView(depthImage.image, depthImage.imageFormat, VK_IMAGE_ASPECT_DEPTH_BIT, vulkanDevices.logicalDevice, depthImage.imageView);

    
    VkCommandBuffer disposableCommandBuffer;
    CommandManager::beginRecordingSingleSubmitCommands(commandPool, vulkanDevices.logicalDevice, disposableCommandBuffer);
    
    Image::transitionImageLayout(depthImage.image, depthImage.imageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, disposableCommandBuffer);
    depthImage.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    
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
