#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Renderer/Offscreen.h>
#include <core/Shader/Depth.h>
#include <core/VulkanInstance/DeviceHandler.h>
#include <core/DisplayManager/SwapchainHandler.h>
#include <core/Logging/ErrorLogger.h>
#include <core/Defaults/Defaults.h>

#include <array>


void Offscreen::OffscreenOperation::generateMemberComponents(int32_t offscreenWidth, int32_t offscreenHeight, uint32_t layerCount, void (*createSpecializedRenderPass)(DeviceHandler::VulkanDevices, VkRenderPass&), void (*createSpecializedPipeline)(VkRenderPass, VkDevice, Pipeline::PipelineComponents&), VkCommandPool graphicsCommandPool, VkQueue graphicsQueue, DeviceHandler::VulkanDevices vulkanDevices)
{
    if (layerCount == 6) {
        uint32_t offscreenArea = std::max(offscreenWidth, offscreenHeight);
        this->offscreenExtent.width = offscreenArea;
        this->offscreenExtent.height = offscreenArea;  // must be equal for cubemap depth map/point shadow mapping.
    } else {
        this->offscreenExtent.width = offscreenWidth;
        this->offscreenExtent.height = offscreenHeight;
    }
    this->renderExtent.width = offscreenWidth;
    this->renderExtent.height = offscreenHeight;

    this->imageViews.resize(layerCount);

    
    if (this->beenGenerated == false) {  // if this is the first time this operation is being generated.        
        Image::createTextureSampler(vulkanDevices, 1, this->depthTextureDetails.textureSampler);

        createSpecializedRenderPass(vulkanDevices, this->renderPass);
        createSpecializedPipeline(this->renderPass, vulkanDevices.logicalDevice, this->pipelineComponents);

        this->beenGenerated = true;
    } else {
        for (VkFramebuffer framebuffer : this->framebuffers) {
            vkDestroyFramebuffer(vulkanDevices.logicalDevice, framebuffer, nullptr);
        }
        
        if (this->imageViews.size() == 6) {
            for (VkImageView imageView : this->imageViews) {
                vkDestroyImageView(vulkanDevices.logicalDevice, imageView, nullptr);
            }
        }
    }

    
    Depth::populateDepthImageDetails(this->offscreenExtent, VK_SAMPLE_COUNT_1_BIT, layerCount, VK_IMAGE_USAGE_SAMPLED_BIT, graphicsCommandPool, graphicsQueue, vulkanDevices, this->depthTextureDetails.textureImageDetails);
    Image::createImageView(this->depthTextureDetails.textureImageDetails.image, this->depthTextureDetails.textureImageDetails.imageFormat, 1, layerCount, VK_IMAGE_ASPECT_DEPTH_BIT, vulkanDevices.logicalDevice, this->depthTextureDetails.textureImageDetails.imageView);
        
    if (layerCount == 6) {
        VkImageViewCreateInfo imageViewCreateInfo{};
        Image::populateImageViewCreateInfo(this->depthTextureDetails.textureImageDetails.image, VK_IMAGE_VIEW_TYPE_2D, this->depthTextureDetails.textureImageDetails.imageFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, -1, 1, imageViewCreateInfo);
            
        Image::createImageView(this->depthTextureDetails.textureImageDetails.image, this->depthTextureDetails.textureImageDetails.imageFormat, 1, layerCount, VK_IMAGE_ASPECT_DEPTH_BIT, vulkanDevices.logicalDevice, this->depthTextureDetails.textureImageDetails.imageView);
        for (size_t i = 0; i < layerCount; i += 1) {
            imageViewCreateInfo.subresourceRange.baseArrayLayer = i;

            VkResult imageViewCreationResult = vkCreateImageView(vulkanDevices.logicalDevice, &imageViewCreateInfo, nullptr, &this->imageViews[i]);
            if (imageViewCreationResult != VK_SUCCESS) {
                throwDebugException("Failed to create a offscreen image view.");
            }
        }
    }
    
    // similar to SwapchainHandler::createSwapchainFramebuffers.
    this->framebuffers.resize(Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT * layerCount);

    for (size_t i = 0; i < layerCount; i += 1) {
        std::array<VkImageView, 1> framebufferAttachments = {(layerCount == 6 ? this->imageViews[i] : this->depthTextureDetails.textureImageDetails.imageView)};

        VkFramebufferCreateInfo framebufferCreateInfo{};
        SwapchainHandler::populateFramebufferCreateInfo(this->renderPass, framebufferAttachments.data(), static_cast<uint32_t>(framebufferAttachments.size()), this->offscreenExtent.width, this->offscreenExtent.height, framebufferCreateInfo);
        for (size_t j = 0; j < Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT; j += 1) {
            VkResult framebufferCreationResult = vkCreateFramebuffer(vulkanDevices.logicalDevice, &framebufferCreateInfo, nullptr, &this->framebuffers[j + (i * Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT)]);
            if (framebufferCreationResult != VK_SUCCESS) {
                throwDebugException("Failed to create a offscreen operation framebuffer.");
            }
        }
    }
}

void Offscreen::OffscreenOperation::cleanupOffscreenOperation(VkDevice vulkanLogicalDevice)
{
    for (VkFramebuffer framebuffer : this->framebuffers) {
        vkDestroyFramebuffer(vulkanLogicalDevice, framebuffer, nullptr);
    }

    if (this->imageViews.size() == 6) {
        for (VkImageView imageView : this->imageViews) {
            vkDestroyImageView(vulkanLogicalDevice, imageView, nullptr);
        }
    }

    this->depthTextureDetails.cleanupTextureDetails(vulkanLogicalDevice);
    
    this->pipelineComponents.cleanupPipelineComponents(vulkanLogicalDevice);

    vkDestroyRenderPass(vulkanLogicalDevice, this->renderPass, nullptr);
}
