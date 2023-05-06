#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <core/Shader/Image.h>
#include <core/VulkanInstance/DeviceHandler.h>
#include <core/Command/CommandManager.h>
#include <core/Buffer/Buffer.h>
#include <core/Logging/ErrorLogger.h>

#include <string>


void Image::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryProperties, DeviceHandler::VulkanDevices vulkanDevices, VkImage& image, VkDeviceMemory& imageMemory)
{
    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;

    imageCreateInfo.extent.width = width;
    imageCreateInfo.extent.height = height;
    imageCreateInfo.extent.depth = 1;

    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;

    imageCreateInfo.format = format;
    imageCreateInfo.tiling = tiling;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;  // we generally transition the image to a transfer destination then copy.

    imageCreateInfo.usage = usage;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;  // only relevant for multisampling.

    VkResult imageCreationResult = vkCreateImage(vulkanDevices.logicalDevice, &imageCreateInfo, nullptr, &image);
    if (imageCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create image.");
    }


    VkMemoryRequirements imageMemoryRequirements;
    vkGetImageMemoryRequirements(vulkanDevices.logicalDevice, image, &imageMemoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    memoryAllocateInfo.allocationSize = imageMemoryRequirements.size;
    uint32_t selectedMemoryTypeIndex;
    Buffer::findBufferMemoryType(vulkanDevices.physicalDevice, imageMemoryRequirements.memoryTypeBits, memoryProperties, selectedMemoryTypeIndex);
    memoryAllocateInfo.memoryTypeIndex = selectedMemoryTypeIndex;

    VkResult memoryAllocationResult = vkAllocateMemory(vulkanDevices.logicalDevice, &memoryAllocationInfo, nullptr, &imageMemory);
    if (memoryAllocationResult != VK_SUCCESS) {
        throwDebugException("Failed to allocate image memory.");
    }

    vkBindImageMemory(vulkanDevices.logicalDevice, image, imageMemory, 0);
}

void Image::createTextureImage(std::string textureImageFilePath, VkCommandPool commandPool, VkQueue commandQueue, DeviceHandler::VulkanDevices vulkanDevices, VkImage& textureImage, VkDeviceMemory& textureImageMemory)
{
    size_t textureImageWidth;
    size_t textureImageHeight;
    size_t textureImageChannels;
    
    stbi_uc *textureImagePixels = stbi_load(textureImageFilePath.c_str(), &textureImageWidth, &textureImageHeight, &textureImageChannels, STBI_rgb_alpha);
    if (!textureImagePixels) {  // if image not loaded.
        throwDebugException("Failed to load texture image.");
    }

    VkDeviceSize textureImageSize = textureImageWidth * textureImageHeight * 4;  // 4 bytes per pixel.


    // similar process to Buffer::createDataBufferComponents, but with an buffer and an image rather than a buffer and a buffer.
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    Buffer::createBufferComponents(textureImageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vulkanDevices, stagingBuffer, stagingBufferMemory);

    // copy texture image pixel data into the staging buffer's memory.
    void *mappedStagingBufferMemory;
    vkMapMemory(vulkanDevices.logicalDevice, stagingBufferMemory, 0, textureImageSize, 0, mappedStagingBufferMemory);
    memcpy(mappedStagingBufferMemory, textureImagePixels, textureImageSize);
    vkUnmapMemory(vulkanDevices.logicalDevice, stagingBufferMemory);

    stbi_image_free(textureImagePixels);


    VkFormat textureImageFormat = VK_FORMAT_R8G8B8A8_SRGB;
        
    Image::createImage(textureImageWidth, textureImageHeight, textureImageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vulkanDevices, textureImage, textureImageMemory);

    // could optimize single-submit commands functions with setup and flush command buffer functions.
    Image::transitionImageLayout(textureImage, textureImageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, commandPool, commandQueue, vulkanDevices.logicalDevice);
    Image::copyBufferToImage(stagingBuffer, textureImage, textureImageWidth, textureImageHeight, commandPool, commandQueue, vulkanDevices.logicalDevice);
    
    Image::transitionImageLayout(textureImage, textureImageFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


    vkDestroyBuffer(vulkanDevices.logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(vulkanDevices.logicalDevice, stagingBufferMemory, nullptr);
}

void Image::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout initialImageLayout, VkImageLayout targetImageLayout, VkCommandPool commandPool, VkQueue commandQueue, VkDevice vulkanLogicalDevice)
{
    VkCommandBuffer disposableCommandBuffer;
    CommandManager::beginSingleSubmitCommands(commandPool, vulkanLogicalDevice, disposableCommandBuffer);


    VkImageMemoryBarrier imageMemoryBarrier{};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

    imageMemoryBarrier.oldLayout = initialLayout;
    imageMemoryBarrier.newLayout = targetLayout;

    // we aren't transferring explicit queue family ownership.
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    imageMemoryBarrier.image = image;
    
    imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    imageMemoryBarrier.subresourceRange.levelCount = 1;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (initialLayout == VK_IMAGE_LAYOUT_UNDEFINED && targetLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {  // transitioning to write image pixels.
        imageMemoryBarrier.srcAccessMask = 0;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (initialLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && targetLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {  // transitioning to read from the fragment shader.
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throwDebugException("Unsupported/invalid layout transition.");
    }

    VkCmdPipelineBarrier(disposableCommandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);


    CommandManager::submitSingleSubmitCommands(disposableCommandBuffer, commandPool, commandQueue, vulkanLogicalDevice);
}

void Image::copyBufferToImage(VkBuffer sourceBuffer, VkImage destinationImage, uint32_t imageWidth, uint32_t imageHeight, VkCommandPool commandPool, VkQueue commandQueue, VkDevice vulkanLogicalDevice)
{
    VkCommandBuffer disposableCommandBuffer;
    CommandManager::beginSingleSubmitCommands(commandPool, vulkanLogicalDevice, disposableCommandBuffer);
    
    
    VkBufferImageCopy bufferImageCopy{};
    bufferImageCopy.imageSubresource.layerCount = 0;

    bufferImageCopy.imageOffset = {0, 0, 0};
    bufferImageCopy.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(disposableCommandBuffer, sourceBuffer, destinationImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopy);


    CommandManager::submitSingleSubmitCommands(disposableCommandBuffer, commandPool, commandQueue, vulkanLogicalDevice);
}
