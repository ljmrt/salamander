#ifndef IMAGE_H
#define IMAGE_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/DeviceHandler.h>

#include <string>


namespace Image
{
    // create an Vulkan image.
    //
    // @param width the image's width.
    // @param height the image's height.
    // @param format the image's format.
    // @param tiling the image's tiling mode.
    // @param usage the image's Vulkan usage.
    // @param memoryProperties the memory properties that the image memory must abide to.
    // @param vulkanDevices Vulkan physical and logical device.
    // @param image created image.
    // @param imageMemory allocated image memory.
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryProperties, DeviceHandler::VulkanDevices vulkanDevices, VkImage& image, VkDeviceMemory& imageMemory);

    // create an Vulkan image to be used as a texture.
    //
    // @param textureFilePath the texture image's (absolute)file path.
    // @param commandPool command pool to use in texture image creation.
    // @param commandQueue queue to submit creation and similar commands on.
    // @param vulkanDevices Vulkan physical and logical device.
    // @param textureImage created texture image.
    // @param textureImageMemory allocated texture image memory.
    void createTextureImage(std::string textureImageFilePath, VkCommandPool commandPool, VkQueue commandQueue, DeviceHandler::VulkanDevices vulkanDevices, VkImage& textureImage, VkDeviceMemory& textureImageMemory);

    // create an Vulkan image view.
    //
    // @param baseImage image to base the image view off of.
    // @param baseFormat the base image's format.
    // @param vulkanLogicalDevice Vulkan logical device to use in image view creation.
    // @param imageView created image view.
    void createImageView(VkImage baseImage, VkFormat baseFormat, VkDevice vulkanLogicalDevice, VkImageView& imageView);

    // create an texture sampler.
    //
    // @param vulkanDevices Vulkan physical and logical device.
    // @param textureSampler created texture sampler.
    void createTextureSampler(DeviceHandler::VulkanDevices vulkanDevices, VkSampler& textureSampler);

    // transition a Vulkan image's layout from one to another.
    //
    // @param image the image to transition the image layout of.
    // @param initialImageLayout the image's initial or starting image layout.
    // @param targetImageLayout the image's target or result image layout.
    // @param commandBuffer command buffer to use in transition operations.
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout initialImageLayout, VkImageLayout targetImageLayout, VkCommandBuffer commandBuffer);

    // copy a buffer with pixel data to an Vulkan image.
    //
    // @param sourceBuffer the source buffer containing the pixel data.
    // @param destinationImage the destination image to be copied to.
    // @param imageWidth the width of the image.
    // @param imageHeight the height of the image.
    // @param commandBuffer command buffer to use in copying operations.
    void copyBufferToImage(VkBuffer sourceBuffer, VkImage destinationImage, uint32_t imageWidth, uint32_t imageHeight, VkCommandBuffer commandBuffer);
}


#endif  // IMAGE_H
