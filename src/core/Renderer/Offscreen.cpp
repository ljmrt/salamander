#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Renderer/Offscreen.h>
#include <core/Shader/Depth.h>
#include <core/VulkanInstance/DeviceHandler.h>
#include <core/DisplayManager/SwapchainHandler.h>
#include <core/Logging/ErrorLogger.h>
#include <core/Defaults/Defaults.h>

#include <array>


void Offscreen::OffscreenOperation::generateMemberComponents(int32_t offscreenWidth, int32_t offscreenHeight, void (*createSpecializedRenderPass)(DeviceHandler::VulkanDevices, VkRenderPass&), void (*createSpecializedPipeline)(VkRenderPass, VkDevice, Pipeline::PipelineComponents&), VkCommandPool graphicsCommandPool, VkQueue graphicsQueue, DeviceHandler::VulkanDevices vulkanDevices)
{
    this->offscreenExtent.width = offscreenWidth;
    this->offscreenExtent.height = offscreenHeight;

    
    Depth::populateDepthImageDetails(this->offscreenExtent, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_SAMPLED_BIT, graphicsCommandPool, graphicsQueue, vulkanDevices, this->depthTextureDetails.textureImageDetails);
    Image::createTextureSampler(vulkanDevices, 1, this->depthTextureDetails.textureSampler);


    createSpecializedRenderPass(vulkanDevices, this->renderPass);
    createSpecializedPipeline(this->renderPass, vulkanDevices.logicalDevice, this->pipelineComponents);


    // similar to SwapchainHandler::createSwapchainFramebuffers.
    this->framebuffers.resize(Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < framebuffers.size(); i += 1) {
        std::array<VkImageView, 1> framebufferAttachments = {this->depthTextureDetails.textureImageDetails.imageView};

        VkFramebufferCreateInfo framebufferCreateInfo{};
        SwapchainHandler::populateFramebufferCreateInfo(this->renderPass, framebufferAttachments.data(), static_cast<uint32_t>(framebufferAttachments.size()), this->offscreenExtent.width, this->offscreenExtent.height, framebufferCreateInfo);
        VkResult framebufferCreationResult = vkCreateFramebuffer(vulkanDevices.logicalDevice, &framebufferCreateInfo, nullptr, &this->framebuffers[i]);
        if (framebufferCreationResult != VK_SUCCESS) {
            throwDebugException("Failed to create a offscreen operation framebuffer.");
        }
    }
}

void Offscreen::OffscreenOperation::cleanupOffscreenOperation(VkDevice vulkanLogicalDevice)
{
    for (VkFramebuffer framebuffer : this->framebuffers) {
        vkDestroyFramebuffer(vulkanLogicalDevice, framebuffer, nullptr);
    }

    this->depthTextureDetails.cleanupTextureDetails(vulkanLogicalDevice);
    
    this->pipelineComponents.cleanupPipelineComponents(vulkanLogicalDevice);

    vkDestroyRenderPass(vulkanLogicalDevice, this->renderPass, nullptr);
}
