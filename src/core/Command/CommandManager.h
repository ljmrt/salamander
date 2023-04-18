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
    // @param graphicsCommandPool stored created graphics command pool.
    void createGraphicsCommandPool(size_t graphicsFamilyIndex, VkDevice vulkanLogicalDevice, VkCommandPool& graphicsCommandPool);

    // allocate am amount of command buffers under a command pool.
    //
    // @param parentCommandPool parent command pool to allocate the child command buffers under(also determines command buffer use).
    // @param commandBufferCount the amount of command buffers to create.
    // @param vulkanLogicalDevice the Vulkan instance's logical device.
    // @param commandBuffers stored created command buffers.
    void allocateChildCommandBuffers(VkCommandPool parentCommandPool, size_t commandBufferCount, VkDevice vulkanLogicalDevice, std::vector<VkCommandBuffer>& childCommandBuffers);

    // record necessary drawing commands in a graphics command buffer.
    //
    // @param graphicsCommandBuffer graphics command buffer to record in.
    // @param renderPass render pass to use in commands.
    // @param swapchainImageFramebuffer the swapchain image's framebuffer to use in commands.
    // @param swapchainImageExtent the swapchain image's extent to use in commands.
    // @param graphicsPipeline graphics pipeline to use in commands.
    void recordGraphicsCommandBufferCommands(VkCommandBuffer graphicsCommandBuffer, VkRenderPass renderPass, VkFramebuffer swapchainImageFramebuffer, VkExtent2D swapchainImageExtent, VkPipeline graphicsPipeline);
}


#endif  // COMMANDMANAGER_H
