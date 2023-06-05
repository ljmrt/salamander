#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Renderer/Renderer.h>

#include <vector>


namespace CommandManager
{
    // create a command pool for graphics command buffers.
    //
    // @param graphicsFamilyIndex index of the graphics queue family.
    // @param vulkanLogicalDevice the Vulkan instance's logical device.
    // @param createdGraphicsCommandPool stored created graphics command pool.
    void createGraphicsCommandPool(size_t graphicsFamilyIndex, VkDevice vulkanLogicalDevice, VkCommandPool& createdGraphicsCommandPool);

    // allocate am amount of command buffers under a command pool.
    //
    // @param parentCommandPool parent command pool to allocate the child command buffers under(also determines command buffer use).
    // @param commandBufferCount the amount of command buffers to create.
    // @param vulkanLogicalDevice the Vulkan instance's logical device.
    // @param allocatedChildCommandBuffers stored allocated child command buffers.
    void allocateChildCommandBuffers(VkCommandPool parentCommandPool, size_t commandBufferCount, VkDevice vulkanLogicalDevice, std::vector<VkCommandBuffer>& allocatedChildCommandBuffers);

    // record single-submit commands(allocates a disposable command buffer).
    //
    // @param parentCommandPool parent command pool to allocate the disposable command buffer from.
    // @param vulkanLogicalDevice Vulkan logical device to use in initial command buffer creation.
    // @param begunCommandBuffer the allocated begun command buffer.
    void beginRecordingSingleSubmitCommands(VkCommandPool parentCommandPool, VkDevice vulkanLogicalDevice, VkCommandBuffer& begunCommandBuffer);

    // submit single-submit commands(submit commands from a disposable command buffer).
    //
    // @param recordedCommandBuffer the disposable recorded command buffer.
    // @param parentCommandPool the parent command pool that initially allocated the command buffer.
    // @param submissionQueue queue to submit the single-submit commands on.
    // @param vulkanLogicalDevice Vulkan logical device to use in single-submit command submission.
    void submitSingleSubmitCommands(VkCommandBuffer recordedCommandBuffer, VkCommandPool parentCommandPool, VkQueue submissionQueue, VkDevice vulkanLogicalDevice);

    // record necessary drawing commands in a graphics command buffer.
    //
    // @param graphicsCommandBuffer graphics command buffer to record in.
    // @param swapchainImageExtent the swapchain image's extent to use in commands.
    // @param swapchainIndexFramebuffer this image index's swapchain framebuffer.
    // @param currentFrame the current renderer-drawing frame to use in commands.
    // @param cubemapPipelineComponents components of the cubemap graphics pipeline.
    // @param cubemapShaderBufferComponents vertex and index buffer used for the cubemap's model.
    // @param scenePipelineComponents components of the scene graphics pipeline.
    // @param sceneShaderBufferComponents vertex and index buffer used for the scene's main model.
    // @param renderPass the render pass to use in graphics operations.
    void recordGraphicsCommandBufferCommands(VkCommandBuffer graphicsCommandBuffer, VkExtent2D swapchainImageExtent, VkFramebuffer swapchainIndexFramebuffer, size_t currentFrame, RendererDetails::PipelineComponents cubemapPipelineComponents, ModelHandler::ShaderBufferComponents cubemapShaderBufferComponents, RendererDetails::PipelineComponents scenePipelineComponents, ModelHandler::ShaderBufferComponents sceneShaderBufferComponents, VkRenderPass renderPass);
}


#endif  // COMMANDMANAGER_H
