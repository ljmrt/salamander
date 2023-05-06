#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Command/CommandManager.h>
#include <core/Logging/ErrorLogger.h>
#include <core/Model/VertexHandler.h>

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

void CommandManager::recordGraphicsCommandBufferCommands(VkCommandBuffer graphicsCommandBuffer, VkRenderPass renderPass, VkFramebuffer swapchainImageFramebuffer, VkExtent2D swapchainImageExtent, VkPipeline graphicsPipeline, VkBuffer vertexBuffer, VkBuffer indexBuffer, VkPipelineLayout pipelineLayout, std::vector<VkDescriptorSet>& descriptorSets, size_t currentFrame)
{
    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    commandBufferBeginInfo.flags = 0;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;

    uint32_t commandBufferBeginResult = vkBeginCommandBuffer(graphicsCommandBuffer, &commandBufferBeginInfo);  // will reset command buffer if already recorded.
    if (commandBufferBeginResult != VK_SUCCESS) {
        throwDebugException("Failed to begin recording graphics command buffer commands.");
    }
    
    
    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.framebuffer = swapchainImageFramebuffer;
    
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = swapchainImageExtent;

    // clear color and info to use in color attachment load op clearing.
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};  // black clear color.
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(graphicsCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);  // embed render pass commands directly into the primary command buffer.

    vkCmdBindPipeline(graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);  // we are using a graphics pipeline.

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

    vkCmdBindPipeline(graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
    VkBuffer vertexBuffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};
    
    vkCmdBindVertexBuffers(graphicsCommandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(graphicsCommandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);

    vkCmdDrawIndexed(graphicsCommandBuffer, static_cast<uint32_t>(VertexHandler::indices.size()), 1, 0, 0, 0);  // command buffer, indice count, instance count, indice index offset, indice add offset, instance index offset.

    vkCmdEndRenderPass(graphicsCommandBuffer);

    uint32_t recordBufferCommandsResult = vkEndCommandBuffer(graphicsCommandBuffer);
    if (recordBufferCommandsResult != VK_SUCCESS) {
        throwDebugException("Failed to end recording graphics command buffer commands.");
    }
}
