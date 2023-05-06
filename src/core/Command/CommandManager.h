#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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
    // @param renderPass render pass to use in commands.
    // @param swapchainImageFramebuffer the swapchain image's framebuffer to use in commands.
    // @param swapchainImageExtent the swapchain image's extent to use in commands.
    // @param graphicsPipeline graphics pipeline to use in commands.
    // @param vertexBuffer vertex buffer to use in commands.
    // @param indexBuffer index buffer to use in commands.
    // @param pipelineLayout pipeline layout to use in commands.
    // @param descriptorSets descriptor sets to use in commands.
    // @param currentFrame the current renderer-drawing frame to use in commands.
    void recordGraphicsCommandBufferCommands(VkCommandBuffer graphicsCommandBuffer, VkRenderPass renderPass, VkFramebuffer swapchainImageFramebuffer, VkExtent2D swapchainImageExtent, VkPipeline graphicsPipeline, VkBuffer vertexBuffer, VkBuffer indexBuffer, VkPipelineLayout pipelineLayout, std::vector<VkDescriptorSet>& descriptorSets, size_t currentFrame);
}


#endif  // COMMANDMANAGER_H
