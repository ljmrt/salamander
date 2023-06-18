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

void CommandManager::populateRenderPassBeginInfo(VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D extent, uint32_t clearValueCount, VkClearValue pClearValues[], VkRenderPassBeginInfo& renderPassBeginInfo)
{
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.framebuffer = framebuffer;
    
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = extent;
    
    renderPassBeginInfo.clearValueCount = clearValueCount;
    renderPassBeginInfo.pClearValues = pClearValues;
}

void CommandManager::populateViewportInfo(float x, float y, float width, float height, float minDepth, float maxDepth, VkViewport& viewportInfo)
{
    viewportInfo.x = x;
    viewportInfo.y = y;
    viewportInfo.width = width;
    viewportInfo.height = height;
    viewportInfo.minDepth = minDepth;
    viewportInfo.maxDepth = maxDepth;
}

void CommandManager::populateRect2DInfo(VkExtent2D extent, VkRect2D& rect2DInfo)
{
    rect2DInfo.offset = {0, 0};
    rect2DInfo.extent = extent;
}

void CommandManager::recordGraphicsCommandBufferCommands(CommandManager::GraphicsRecordingPackage graphicsRecordingPackage)
{
    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    commandBufferBeginInfo.flags = 0;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;

    uint32_t commandBufferBeginResult = vkBeginCommandBuffer(graphicsRecordingPackage.graphicsCommandBuffer, &commandBufferBeginInfo);  // will reset command buffer if already recorded.
    if (commandBufferBeginResult != VK_SUCCESS) {
        throwDebugException("Failed to begin recording graphics command buffer commands.");
    }
    

    // attachment clear values are used in load operation clearing.
    VkClearValue colorAttachmentClearValue = {{{1.0f, 1.0f, 1.0f, 1.0f}}};  // clear to white.
    VkClearValue depthAttachmentClearValue = {1.0f, 0};
    std::array<VkClearValue, 1> offscreenAttachmentClearValues = {depthAttachmentClearValue};
    std::array<VkClearValue, 2> mainAttachmentClearValues = {colorAttachmentClearValue, depthAttachmentClearValue};

    
    VkRenderPassBeginInfo offscreenRenderPassBeginInfo{};
    CommandManager::populateRenderPassBeginInfo(graphicsRecordingPackage.offscreenOperation.renderPass, graphicsRecordingPackage.offscreenOperation.framebuffers[graphicsRecordingPackage.currentFrame], graphicsRecordingPackage.offscreenOperation.offscreenExtent, static_cast<uint32_t>(offscreenAttachmentClearValues.size()), offscreenAttachmentClearValues.data(), offscreenRenderPassBeginInfo);

    vkCmdBeginRenderPass(graphicsRecordingPackage.graphicsCommandBuffer, &offscreenRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    // TODO: is this necessary for an offsceen operation?
    // set our dynamic pipeline states.
    VkViewport offscreenDynamicViewport{};
    CommandManager::populateViewportInfo(0.0f, 0.0f, static_cast<float>(graphicsRecordingPackage.offscreenOperation.offscreenExtent.width), static_cast<float>(graphicsRecordingPackage.offscreenOperation.offscreenExtent.height), 0.0f, 1.0f, offscreenDynamicViewport);
    vkCmdSetViewport(graphicsRecordingPackage.graphicsCommandBuffer, 0, 1, &offscreenDynamicViewport);

    VkRect2D offscreenDynamicScissor{};
    CommandManager::populateRect2DInfo(graphicsRecordingPackage.offscreenOperation.offscreenExtent, offscreenDynamicScissor);
    vkCmdSetScissor(graphicsRecordingPackage.graphicsCommandBuffer, 0, 1, &offscreenDynamicScissor);

    VkDeviceSize offscreenOffsets[] = {0};

    // draw/populate the depth map.
    vkCmdBindVertexBuffers(graphicsRecordingPackage.graphicsCommandBuffer, 0, 1, &graphicsRecordingPackage.offscreenShaderBufferComponents.vertexBuffer, offscreenOffsets);
    vkCmdBindIndexBuffer(graphicsRecordingPackage.graphicsCommandBuffer, graphicsRecordingPackage.offscreenShaderBufferComponents.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(graphicsRecordingPackage.graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsRecordingPackage.offscreenOperation.pipelineComponents.pipelineLayout, 0, 1, &graphicsRecordingPackage.offscreenOperation.pipelineComponents.descriptorSets[graphicsRecordingPackage.currentFrame], 0, nullptr);
    vkCmdBindPipeline(graphicsRecordingPackage.graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsRecordingPackage.offscreenOperation.pipelineComponents.pipeline);

    vkCmdDrawIndexed(graphicsRecordingPackage.graphicsCommandBuffer, graphicsRecordingPackage.offscreenShaderBufferComponents.indiceCount, 1, 0, 0, 0);  // command buffer, indice count, instance count, indice index offset, indice add offset, instance index offset.    

    vkCmdEndRenderPass(graphicsRecordingPackage.graphicsCommandBuffer);
    
    
    VkRenderPassBeginInfo mainRenderPassBeginInfo{};
    CommandManager::populateRenderPassBeginInfo(graphicsRecordingPackage.renderPass, graphicsRecordingPackage.swapchainIndexFramebuffer, graphicsRecordingPackage.swapchainImageExtent, static_cast<uint32_t>(mainAttachmentClearValues.size()), mainAttachmentClearValues.data(), mainRenderPassBeginInfo);

    vkCmdBeginRenderPass(graphicsRecordingPackage.graphicsCommandBuffer, &mainRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);  // embed render pass commands directly into the primary command buffer.

    // set our dynamic pipeline states.
    VkViewport mainDynamicViewport{};
    CommandManager::populateViewportInfo(0.0f, 0.0f, static_cast<float>(graphicsRecordingPackage.swapchainImageExtent.width), static_cast<float>(graphicsRecordingPackage.swapchainImageExtent.height), 0.0f, 1.0f, mainDynamicViewport);
    vkCmdSetViewport(graphicsRecordingPackage.graphicsCommandBuffer, 0, 1, &mainDynamicViewport);

    VkRect2D mainDynamicScissor{};
    CommandManager::populateRect2DInfo(graphicsRecordingPackage.swapchainImageExtent, mainDynamicScissor);
    vkCmdSetScissor(graphicsRecordingPackage.graphicsCommandBuffer, 0, 1, &mainDynamicScissor);

    VkDeviceSize mainOffsets[] = {0};

    // TODO: draw after to prevent overdraw.
    // draw the cubemap.
    vkCmdBindVertexBuffers(graphicsRecordingPackage.graphicsCommandBuffer, 0, 1, &graphicsRecordingPackage.cubemapShaderBufferComponents.vertexBuffer, mainOffsets);
    vkCmdBindIndexBuffer(graphicsRecordingPackage.graphicsCommandBuffer, graphicsRecordingPackage.cubemapShaderBufferComponents.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(graphicsRecordingPackage.graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsRecordingPackage.cubemapPipelineComponents.pipelineLayout, 0, 1, &graphicsRecordingPackage.cubemapPipelineComponents.descriptorSets[graphicsRecordingPackage.currentFrame], 0, nullptr);
    vkCmdBindPipeline(graphicsRecordingPackage.graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsRecordingPackage.cubemapPipelineComponents.pipeline);

    vkCmdDrawIndexed(graphicsRecordingPackage.graphicsCommandBuffer, graphicsRecordingPackage.cubemapShaderBufferComponents.indiceCount, 1, 0, 0, 0);  // command buffer, indice count, instance count, indice index offset, indice add offset, instance index offset.

    // draw the scene.
    vkCmdBindVertexBuffers(graphicsRecordingPackage.graphicsCommandBuffer, 0, 1, &graphicsRecordingPackage.sceneShaderBufferComponents.vertexBuffer, mainOffsets);
    vkCmdBindIndexBuffer(graphicsRecordingPackage.graphicsCommandBuffer, graphicsRecordingPackage.sceneShaderBufferComponents.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(graphicsRecordingPackage.graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsRecordingPackage.scenePipelineComponents.pipelineLayout, 0, 1, &graphicsRecordingPackage.scenePipelineComponents.descriptorSets[graphicsRecordingPackage.currentFrame], 0, nullptr);
    vkCmdBindPipeline(graphicsRecordingPackage.graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsRecordingPackage.scenePipelineComponents.pipeline);

    vkCmdDrawIndexed(graphicsRecordingPackage.graphicsCommandBuffer, graphicsRecordingPackage.sceneShaderBufferComponents.indiceCount, 1, 0, 0, 0);

    // draw the scene normals.
    vkCmdBindVertexBuffers(graphicsRecordingPackage.graphicsCommandBuffer, 0, 1, &graphicsRecordingPackage.sceneNormalsShaderBufferComponents.vertexBuffer, mainOffsets);
    vkCmdBindIndexBuffer(graphicsRecordingPackage.graphicsCommandBuffer, graphicsRecordingPackage.sceneNormalsShaderBufferComponents.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    
    vkCmdBindDescriptorSets(graphicsRecordingPackage.graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsRecordingPackage.sceneNormalsPipelineComponents.pipelineLayout, 0, 1, &graphicsRecordingPackage.sceneNormalsPipelineComponents.descriptorSets[graphicsRecordingPackage.currentFrame], 0, nullptr);
    vkCmdBindPipeline(graphicsRecordingPackage.graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsRecordingPackage.sceneNormalsPipelineComponents.pipeline);

    vkCmdDrawIndexed(graphicsRecordingPackage.graphicsCommandBuffer, graphicsRecordingPackage.sceneShaderBufferComponents.indiceCount, 1, 0, 0, 0);
    
    vkCmdEndRenderPass(graphicsRecordingPackage.graphicsCommandBuffer);
    

    uint32_t recordBufferCommandsResult = vkEndCommandBuffer(graphicsRecordingPackage.graphicsCommandBuffer);
    if (recordBufferCommandsResult != VK_SUCCESS) {
        throwDebugException("Failed to end recording graphics command buffer commands.");
    }
}
