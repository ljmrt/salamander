#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Command/CommandManager.h>
#include <core/Renderer/Renderer.h>
#include <core/Logging/ErrorLogger.h>
#include <core/Model/ModelHandler.h>

#include <vector>


void CommandManager::createGraphicsCommandPool(size_t graphicsFamilyIndex, VkDevice vulkanLogicalDevice, VkCommandPool& createdGraphicsCommandPool)
{
    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;  // rerecord command buffers individually.
    commandPoolCreateInfo.queueFamilyIndex = graphicsFamilyIndex;

    uint32_t commandPoolCreationResult = vkCreateCommandPool(vulkanLogicalDevice, &commandPoolCreateInfo, nullptr, &createdGraphicsCommandPool);
    if (commandPoolCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create graphics command pool.");
    }
}

void CommandManager::allocateChildCommandBuffers(VkCommandPool parentCommandPool, size_t commandBufferCount, VkDevice vulkanLogicalDevice, std::vector<VkCommandBuffer>& allocatedChildCommandBuffers)
{
    allocatedChildCommandBuffers.resize(commandBufferCount);
    
    VkCommandBufferAllocateInfo commandBufferAllocationInfo{};
    commandBufferAllocationInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;

    commandBufferAllocationInfo.commandPool = parentCommandPool;
    commandBufferAllocationInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;  // buffers can be submitted directly to queue.
    commandBufferAllocationInfo.commandBufferCount = commandBufferCount;

    uint32_t commandBufferCreationResult = vkAllocateCommandBuffers(vulkanLogicalDevice, &commandBufferAllocationInfo, allocatedChildCommandBuffers.data());
    if (commandBufferCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to allocate child command buffers.");
    }
}

void CommandManager::beginRecordingSingleSubmitCommands(VkCommandPool parentCommandPool, VkDevice vulkanLogicalDevice, VkCommandBuffer& begunCommandBuffer)
{
    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    
    commandBufferAllocateInfo.commandPool = parentCommandPool;
    commandBufferAllocateInfo.commandBufferCount = 1;

    vkAllocateCommandBuffers(vulkanLogicalDevice, &commandBufferAllocateInfo, &begunCommandBuffer);


    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;  // command buffer only to be used/submitted once.

    vkBeginCommandBuffer(begunCommandBuffer, &commandBufferBeginInfo);
}

void CommandManager::submitSingleSubmitCommands(VkCommandBuffer recordedCommandBuffer, VkCommandPool parentCommandPool, VkQueue submissionQueue, VkDevice vulkanLogicalDevice)
{
    vkEndCommandBuffer(recordedCommandBuffer);


    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &recordedCommandBuffer;

    vkQueueSubmit(submissionQueue, 1, &submitInfo, VK_NULL_HANDLE);

    
    vkQueueWaitIdle(submissionQueue);
    vkFreeCommandBuffers(vulkanLogicalDevice, parentCommandPool, 1, &recordedCommandBuffer);
}

void CommandManager::recordGraphicsCommandBufferCommands(VkCommandBuffer graphicsCommandBuffer, VkFramebuffer swapchainImageFramebuffer, VkExtent2D swapchainImageExtent, PipelineComponents scenePipelineComponents, ModelHandler::ShaderBufferComponents sceneShaderBufferComponents, PipelineComponents cubemapPipelineComponents, ModelHandler::ShaderBufferComponents cubemapShaderBufferComponents, size_t currentFrame)
{
    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    commandBufferBeginInfo.flags = 0;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;

    uint32_t commandBufferBeginResult = vkBeginCommandBuffer(graphicsCommandBuffer, &commandBufferBeginInfo);  // will reset command buffer if already recorded.
    if (commandBufferBeginResult != VK_SUCCESS) {
        throwDebugException("Failed to begin recording graphics command buffer commands.");
    }


    // TODO: abstract this to a seperate function.
    VkRenderPassBeginInfo cubemapRenderPassBeginInfo{};
    cubemapRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

    cubemapRenderPassBeginInfo.renderPass = cubemapPipelineComponents.renderPass;
    cubemapRenderPassBeginInfo.framebuffer = swapchainImageFramebuffer;
    
    cubemapRenderPassBeginInfo.renderArea.offset = {0, 0};
    cubemapRenderPassBeginInfo.renderArea.extent = swapchainImageExtent;

    // attachment clear values are used in load operation clearing.
    VkClearValue cubemapColorAttachmentClearValue = {{{0.0f, 0.0f, 0.0f, 1.0f}}};  // black clear color.
    std::array<VkClearValue, 1> cubemapAttachmentClearValues = {cubemapColorAttachmentClearValue};
    
    cubemapRenderPassBeginInfo.clearValueCount = static_cast<uint32_t>(cubemapAttachmentClearValues.size());
    cubemapRenderPassBeginInfo.pClearValues = cubemapAttachmentClearValues.data();

    vkCmdBeginRenderPass(graphicsCommandBuffer, &cubemapRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);  // embed render pass commands directly into the primary command buffer.

    // set our dynamic pipeline states.
    VkViewport cubemapDynamicViewport{};
    cubemapDynamicViewport.x = 0.0f;
    cubemapDynamicViewport.y = 0.0f;
    cubemapDynamicViewport.width = static_cast<float>(swapchainImageExtent.width);
    cubemapDynamicViewport.height = static_cast<float>(swapchainImageExtent.height);
    cubemapDynamicViewport.minDepth = 0.0f;
    cubemapDynamicViewport.maxDepth = 1.0f;
    vkCmdSetViewport(graphicsCommandBuffer, 0, 1, &cubemapDynamicViewport);

    VkRect2D cubemapDynamicScissor{};
    cubemapDynamicScissor.offset = {0, 0};
    cubemapDynamicScissor.extent = swapchainImageExtent;
    vkCmdSetScissor(graphicsCommandBuffer, 0, 1, &cubemapDynamicScissor);

    VkDeviceSize cubemapOffsets[] = {0};
    
    // draw the cubemap.
    vkCmdBindVertexBuffers(graphicsCommandBuffer, 0, 1, &cubemapShaderBufferComponents.vertexBuffer, cubemapOffsets);
    vkCmdBindIndexBuffer(graphicsCommandBuffer, cubemapShaderBufferComponents.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, cubemapPipelineComponents.pipelineLayout, 0, 1, &cubemapPipelineComponents.descriptorSets[currentFrame], 0, nullptr);
    vkCmdBindPipeline(graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, cubemapPipelineComponents.pipeline);

    vkCmdDrawIndexed(graphicsCommandBuffer, cubemapShaderBufferComponents.indiceCount, 1, 0, 0, 0);  // command buffer, indice count, instance count, indice index offset, indice add offset, instance index offset.
    
    vkCmdEndRenderPass(graphicsCommandBuffer);
    
    
    VkRenderPassBeginInfo sceneRenderPassBeginInfo{};
    sceneRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

    sceneRenderPassBeginInfo.renderPass = scenePipelineComponents.renderPass;
    sceneRenderPassBeginInfo.framebuffer = swapchainImageFramebuffer;
    
    sceneRenderPassBeginInfo.renderArea.offset = {0, 0};
    sceneRenderPassBeginInfo.renderArea.extent = swapchainImageExtent;

    // attachment clear values are used in load operation clearing.
    VkClearValue sceneColorAttachmentClearValue = {{{0.0f, 0.0f, 0.0f, 1.0f}}};  // black clear color.
    VkClearValue sceneDepthAttachmentClearValue = {1.0f, 0};
    std::array<VkClearValue, 2> sceneAttachmentClearValues = {sceneColorAttachmentClearValue, sceneDepthAttachmentClearValue};
    
    sceneRenderPassBeginInfo.clearValueCount = static_cast<uint32_t>(sceneAttachmentClearValues.size());
    sceneRenderPassBeginInfo.pClearValues = sceneAttachmentClearValues.data();

    vkCmdBeginRenderPass(graphicsCommandBuffer, &sceneRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);  // embed render pass commands directly into the primary command buffer.

    // set our dynamic pipeline states.
    VkViewport sceneDynamicViewport{};
    sceneDynamicViewport.x = 0.0f;
    sceneDynamicViewport.y = 0.0f;
    sceneDynamicViewport.width = static_cast<float>(swapchainImageExtent.width);
    sceneDynamicViewport.height = static_cast<float>(swapchainImageExtent.height);
    sceneDynamicViewport.minDepth = 0.0f;
    sceneDynamicViewport.maxDepth = 1.0f;
    vkCmdSetViewport(graphicsCommandBuffer, 0, 1, &sceneDynamicViewport);

    VkRect2D sceneDynamicScissor{};
    sceneDynamicScissor.offset = {0, 0};
    sceneDynamicScissor.extent = swapchainImageExtent;
    vkCmdSetScissor(graphicsCommandBuffer, 0, 1, &sceneDynamicScissor);

    VkDeviceSize sceneOffsets[] = {0};

    // draw the scene.
    vkCmdBindVertexBuffers(graphicsCommandBuffer, 0, 1, &sceneShaderBufferComponents.vertexBuffer, sceneOffsets);
    vkCmdBindIndexBuffer(graphicsCommandBuffer, sceneShaderBufferComponents.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, scenePipelineComponents.pipelineLayout, 0, 1, &scenePipelineComponents.descriptorSets[currentFrame], 0, nullptr);
    vkCmdBindPipeline(graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, scenePipelineComponents.pipeline);

    vkCmdDrawIndexed(graphicsCommandBuffer, sceneShaderBufferComponents.indiceCount, 1, 0, 0, 0);  // command buffer, indice count, instance count, indice index offset, indice add offset, instance index offset.

    vkCmdEndRenderPass(graphicsCommandBuffer);
    

    uint32_t recordBufferCommandsResult = vkEndCommandBuffer(graphicsCommandBuffer);
    if (recordBufferCommandsResult != VK_SUCCESS) {
        throwDebugException("Failed to end recording graphics command buffer commands.");
    }
}
