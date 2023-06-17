#ifndef DEPTH_H
#define DEPTH_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/DeviceHandler.h>
#include <core/Shader/Image.h>


namespace Depth
{
    // create depth image, allocate depth image memory, and create depth image view, populate image details with such.
    //
    // @param swapchainImageExtent swapchain image extent to use in depth component creation.
    // @param msaaSampleCount the amount of msaa samples.
    // @param additionalImageUsage additional image usage to VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    // @param commandPool command pool to use in command buffer creation.
    // @param commandQueue queue to submit necessary commands on.
    // @param vulkanDevices Vulkan physical and logical device to use in depth component creation.
    // @param depthImageDetails populated depth image details.
    void populateDepthImageDetails(VkExtent2D swapchainImageExtent, VkSampleCountFlagBits msaaSampleCount, VkImageUsageFlagBits additionalImageUsage, VkCommandPool commandPool, VkQueue commandQueue, DeviceHandler::VulkanDevices vulkanDevices, Image::ImageDetails& depthImageDetails);

    // select a depth image format.
    //
    // @param vulkanPhysicalDevice Vulkan physical device to use in depth image format selection.
    // @param depthImageFormat selected depth image format.
    void selectDepthImageFormat(VkPhysicalDevice vulkanPhysicalDevice, VkFormat& depthImageFormat);

    // test the stencil component availibility of a depth image format.
    //
    // @param depthImageFormat tested depth image format.
    // @return stencil component availibility.
    bool depthImageFormatHasStencilComponent(VkFormat depthImageFormat);
}


#endif  // DEPTH_H
