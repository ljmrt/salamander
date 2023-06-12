#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Renderer/Renderer.h>

#include <vector>


namespace CommandManager
{
    struct GraphicsRecordingPackage {  // contains all necessary data for the "recordGraphicsCommandBufferCommands" function.
        VkCommandBuffer graphicsCommandBuffer;
        VkRenderPass renderPass;
        
        VkExtent2D swapchainImageExtent;
        VkFramebuffer swapchainIndexFramebuffer;
        
        uint32_t currentFrame;
        
        RendererDetails::PipelineComponents cubemapPipelineComponents;
        ModelHandler::ShaderBufferComponents cubemapShaderBufferComponents;
        RendererDetails::PipelineComponents scenePipelineComponents;
        ModelHandler::ShaderBufferComponents sceneShaderBufferComponents;
        RendererDetails::PipelineComponents sceneNormalsPipelineComponents;
        ModelHandler::ShaderBufferComponents sceneNormalsShaderBufferComponents;
    };
    
    
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

    // TODO: large namespace containing all necessary "initializers" for Vulkan structs.
    // TODO: modify "initializer" functions to return the specified struct.
    // populate render pass begin info.
    //
    // @param renderPass see VkRenderPassBeginInfo documentation.
    // @param framebuffer see VkRenderPassBeginInfo documentation.
    // @param extent see VkRenderPassBeginInfo documentation.
    // @param clearValueCount see VkRenderPassBeginInfo documentation.
    // @param pClearValues see VkRenderPassBeginInfo documentation.
    // @param renderPassBeginInfo populated render pass begin info.
    void populateRenderPassBeginInfo(VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D extent, uint32_t clearValueCount, VkClearValue pClearValues[], VkRenderPassBeginInfo& renderPassBeginInfo);

    // populate viewport info.
    //
    // @param x see VkViewport documentation.
    // @param y see VkViewport documentation.
    // @param width see VkViewport documentation.
    // @param height see VkViewport documentation.
    // @param minDepth see VkViewport documentation.
    // @param maxDepth see VkViewport documentation.
    // @param viewportInfo populated viewport info.
    void populateViewportInfo(float x, float y, float width, float height, float minDepth, float maxDepth, VkViewport& viewportInfo);

    // @param extent see VkRect2D documentation.
    // @param rect2DInfo populated Rect2D info.
    void populateRect2DInfo(VkExtent2D extent, VkRect2D& rect2DInfo);

    // record necessary drawing commands in a graphics command buffer.
    //
    // @param graphicsRecordingPackage a package of all necessary data for this function.
    void recordGraphicsCommandBufferCommands(CommandManager::GraphicsRecordingPackage graphicsRecordingPackage);
}


#endif  // COMMANDMANAGER_H
