#ifndef IMAGE_H
#define IMAGE_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/DeviceHandler.h>

#include <string>


namespace Image
{
    // currently not writing documentation due to the refactoring of these functions in the near future.
    
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryProperties, DeviceHandler::VulkanDevices vulkanDevices, VkImage& image, VkDeviceMemory& imageMemory);
    
    void createTextureImage(std::string textureImageFilePath, VkCommandPool commandPool, VkQueue commandQueue, DeviceHandler::VulkanDevices vulkanDevices, VkImage& textureImage, VkDeviceMemory& textureImageMemory);

    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout initialImageLayout, VkImageLayout targetImageLayout, VkCommandPool commandPool, VkQueue commandQueue, VkDevice vulkanLogicalDevice);

    void copyBufferToImage(VkBuffer sourceBuffer, VkImage destinationImage, uint32_t imageWidth, uint32_t imageHeight, VkCommandPool commandPool, VkQueue commandQueue, VkDevice vulkanLogicalDevice);
}


#endif  // IMAGE_H
