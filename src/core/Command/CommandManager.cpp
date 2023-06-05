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

void CommandManager::recordGraphicsCommandBufferCommands(VkCommandBuffer graphicsCommandBuffer, VkExtent2D swapchainImageExtent, VkFramebuffer swapchainIndexFramebuffer, size_t currentFrame, RendererDetails::PipelineComponents cubemapPipelineComponents, ModelHandler::ShaderBufferComponents cubemapShaderBufferComponents, RendererDetails::PipelineComponents scenePipelineComponents, ModelHandler::ShaderBufferComponents sceneShaderBufferComponents, VkRenderPass renderPass)
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
    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.framebuffer = swapchainIndexFramebuffer;
    
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = swapchainImageExtent;

    // attachment clear values are used in load operation clearing.
    VkClearValue colorAttachmentClearValue = {{{1.0f, 1.0f, 1.0f, 1.0f}}};  // clear to white.
    VkClearValue depthAttachmentClearValue = {1.0f, 0};
    std::array<VkClearValue, 2> attachmentClearValues = {colorAttachmentClearValue, depthAttachmentClearValue};
    
    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(attachmentClearValues.size());
    renderPassBeginInfo.pClearValues = attachmentClearValues.data();

    vkCmdBeginRenderPass(graphicsCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);  // embed render pass commands directly into the primary command buffer.

    // set our dynamic pipeline states.
    VkViewport dynamicViewport{};
    dynamicViewport.x = 0.0f;
    dynamicViewport.y = 0.0f;
    dynamicViewport.width = static_cast<float>(swapchainImageExtent.width);
    dynamicViewport.height = static_cast<float>(swapchainImageExtent.height);
    dynamicViewport.minDepth = 0.0f;
    dynamicViewport.maxDepth = 1.0f;
    vkCmdSetViewport(graphicsCommandBuffer, 0, 1, &dynamicViewport);

    VkRect2D dynamicScissor{};
    dynamicScissor.offset = {0, 0};
    dynamicScissor.extent = swapchainImageExtent;
    vkCmdSetScissor(graphicsCommandBuffer, 0, 1, &dynamicScissor);

    VkDeviceSize offsets[] = {0};

    // draw the cubemap.
    vkCmdBindVertexBuffers(graphicsCommandBuffer, 0, 1, &cubemapShaderBufferComponents.vertexBuffer, offsets);
    vkCmdBindIndexBuffer(graphicsCommandBuffer, cubemapShaderBufferComponents.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, cubemapPipelineComponents.pipelineLayout, 0, 1, &cubemapPipelineComponents.descriptorSets[currentFrame], 0, nullptr);
    vkCmdBindPipeline(graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, cubemapPipelineComponents.pipeline);

    vkCmdDrawIndexed(graphicsCommandBuffer, cubemapShaderBufferComponents.indiceCount, 1, 0, 0, 0);  // command buffer, indice count, instance count, indice index offset, indice add offset, instance index offset.

    // draw the scene.
    vkCmdBindVertexBuffers(graphicsCommandBuffer, 0, 1, &sceneShaderBufferComponents.vertexBuffer, offsets);
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
