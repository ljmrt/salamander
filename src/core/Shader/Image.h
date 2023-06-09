#ifndef IMAGE_H
#define IMAGE_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/DeviceHandler.h>

#include <string>


namespace DisplayManager  // forward declaration.
{
    struct DisplayDetails;
}

namespace Image
{
    struct ImageDetails {
        VkImage image;
        VkDeviceMemory imageMemory;
        VkImageView imageView;
        VkImageLayout imageLayout;

        // prefer to use size_t, but better to conform to function requirements.
        int imageWidth;
        int imageHeight;
        int imageChannels;
        VkFormat imageFormat;

        uint32_t mipmapLevels;
        uint32_t imageLayerCount;


        // cleanup this image details.
        //
        // @param vulkanLogicalDevice Vulkan logical device to use in image details cleanup.
        void cleanupImageDetails(VkDevice vulkanLogicalDevice);
    };
    struct TextureDetails {
        Image::ImageDetails textureImageDetails;  // guarenteed to be completely populated after Image::populateTextureDetails.
        
        VkSampler textureSampler;


        // cleanup this texture details.
        //
        // @param vulkanLogicalDevice Vulkan logical device to use in texture details cleanup.
        void cleanupTextureDetails(VkDevice vulkanLogicalDevice);
    };
    
    // populate an image struct.
    //
    // @param width the image's width.
    // @param height the image's height.
    // @param format the image's format.
    // @param mipmapLevels the amount of mipmap levels.
    // @param layerCount the amount of image layers.
    // @param msaaSampleCount the amount of msaa samples.
    // @param tiling the image's tiling mode.
    // @param usage the image's Vulkan usage.
    // @param memoryProperties the memory properties that the image memory must abide to.
    // @param vulkanDevices Vulkan physical and logical device.
    // @param image populated image details.
    // @param optionalImageMipmapLevels optional stored image mipmap levels.
    void populateImageDetails(uint32_t width, uint32_t height, uint32_t mipmapLevels, uint32_t layerCount, VkSampleCountFlagBits msaaSampleCount, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryProperties, DeviceHandler::VulkanDevices vulkanDevices, Image::ImageDetails& imageDetails);

    // populate an texture struct.
    //
    // @param textureFilePath the texture image's (absolute)file path.
    // @param isCubemap if the texture details belong to a cubemap.
    // @param commandPool command pool to use in texture image creation.
    // @param commandQueue queue to submit creation and similar commands on.
    // @param vulkanDevices Vulkan physical and logical device.
    // @param texture populated texture details.
    void populateTextureDetails(std::string textureImageFilePath, bool isCubemap, VkCommandPool commandPool, VkQueue commandQueue, DeviceHandler::VulkanDevices vulkanDevices, Image::TextureDetails& textureDetails);

    // generate the image details necessary for the swapchain.
    //
    // @param displayDetails display details to use in swapchain image details generation.
    // @param vulkanDevices Vulkan physical and logical device.
    void generateSwapchainImageDetails(DisplayManager::DisplayDetails& displayDetails, DeviceHandler::VulkanDevices vulkanDevices);

    // TODO: store/read from stored mipmaps file.
    // generate mipmap levels for image details.
    //
    // @param imageDetails image details to generate and store mipmap levels in.
    // @param commandPool command pool to allocate a command buffer from.
    // @param commandQueue queue to use in the submission of necessary commands.
    // @param vulkanDevices Vulkan logical and physical device to use in mipmap levels generation.
    void generateMipmapLevels(Image::ImageDetails& imageDetails, VkCommandPool commandPool, VkQueue commandQueue, DeviceHandler::VulkanDevices vulkanDevices);

    // create an Vulkan image view.
    //
    // @param baseImage image to base the image view off of.
    // @param baseFormat the base image's format.
    // @param mipmapLevels the amount of mipmap levels.
    // @param layerCount the amount of layers in the image.
    // @param imageAspectFlags image aspect flags to use in image view creation.
    // @param vulkanLogicalDevice Vulkan logical device to use in image view creation.
    // @param imageView created image view.
    void createImageView(VkImage baseImage, VkFormat baseFormat, uint32_t mipmapLevels, uint32_t layerCount, VkImageAspectFlags imageAspectFlags, VkDevice vulkanLogicalDevice, VkImageView& imageView);

    // create an texture sampler.
    //
    // @param vulkanDevices Vulkan physical and logical device.
    // @param mipmapLevels the amount of mipmap levels in the texture image.
    // @param textureSampler created texture sampler.
    void createTextureSampler(DeviceHandler::VulkanDevices vulkanDevices, uint32_t mipmapLevels, VkSampler& textureSampler);

    // select a supported image format out of the candidate formats conformant to image format flags.
    //
    // @param candidateImageFormats the image formats to select from.
    // @param imageTiling the tiling of the image using the selected format.
    // @param imageFormatFeatureFlags image format feature flags for the image format to conform to.
    // @param vulkanPhysicalDevice Vulkan physical device to use in supported image format selection.
    // @param imageFormat selected supported image format.
    void selectSupportedImageFormat(const std::vector<VkFormat> candidateImageFormats, VkImageTiling imageTiling, VkFormatFeatureFlags imageFormatFeatureFlags, VkPhysicalDevice vulkanPhysicalDevice, VkFormat& imageFormat);

    // transition a Vulkan image's layout from one to another.
    //
    // @param image the image to transition the image layout of.
    // @param format the image's format.
    // @param mipmapLevels the amount of mipmap levels in the image.
    // @param layerCount the amount of layers in the image.
    // @param initialImageLayout the image's initial or starting image layout.
    // @param targetImageLayout the image's target or result image layout.
    // @param commandBuffer command buffer to use in transition operations.
    void transitionImageLayout(VkImage image, VkFormat format, uint32_t mipmapLevels, uint32_t layerCount, VkImageLayout initialImageLayout, VkImageLayout targetImageLayout, VkCommandBuffer commandBuffer);

    // copy a buffer with pixel data to an Vulkan image.
    //
    // @param sourceBuffer the source buffer containing the pixel data.
    // @param destinationImage the destination image to be copied to.
    // @param imageWidth the width of the image.
    // @param imageHeight the height of the image.
    // @param imageLayerCount the amount of layers in the image.
    // @param commandBuffer command buffer to use in copying operations.
    void copyBufferToImage(VkBuffer sourceBuffer, VkImage destinationImage, uint32_t imageWidth, uint32_t imageHeight, uint32_t imageLayerCount, VkCommandBuffer commandBuffer);
}


#endif  // IMAGE_H
