#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Command/CommandManager.h>
#include <core/Renderer/Renderer.h>
#include <core/Shader/Shader.h>
#include <core/Shader/Uniform.h>
#include <core/Logging/ErrorLogger.h>
#include <core/Model/ModelHandler.h>
#include <core/Defaults/Defaults.h>

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
    std::array<VkClearValue, 1> directionalShadowAttachmentClearValues = {depthAttachmentClearValue};
    std::array<VkClearValue, 1> pointShadowAttachmentClearValues = {depthAttachmentClearValue};
    std::array<VkClearValue, 2> mainAttachmentClearValues = {colorAttachmentClearValue, depthAttachmentClearValue};

    
    VkRenderPassBeginInfo directionalShadowRenderPassBeginInfo{};
    CommandManager::populateRenderPassBeginInfo(graphicsRecordingPackage.directionalShadowOperation.renderPass, graphicsRecordingPackage.directionalShadowOperation.framebuffers[graphicsRecordingPackage.currentFrame], graphicsRecordingPackage.directionalShadowOperation.offscreenExtent, static_cast<uint32_t>(directionalShadowAttachmentClearValues.size()), directionalShadowAttachmentClearValues.data(), directionalShadowRenderPassBeginInfo);

    vkCmdBeginRenderPass(graphicsRecordingPackage.graphicsCommandBuffer, &directionalShadowRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    // set our dynamic pipeline states.
    VkViewport directionalShadowDynamicViewport{};
    CommandManager::populateViewportInfo(0.0f, 0.0f, static_cast<float>(graphicsRecordingPackage.directionalShadowOperation.offscreenExtent.width), static_cast<float>(graphicsRecordingPackage.directionalShadowOperation.offscreenExtent.height), 0.0f, 1.0f, directionalShadowDynamicViewport);
    vkCmdSetViewport(graphicsRecordingPackage.graphicsCommandBuffer, 0, 1, &directionalShadowDynamicViewport);

    VkRect2D directionalShadowDynamicScissor{};
    CommandManager::populateRect2DInfo(graphicsRecordingPackage.directionalShadowOperation.offscreenExtent, directionalShadowDynamicScissor);
    vkCmdSetScissor(graphicsRecordingPackage.graphicsCommandBuffer, 0, 1, &directionalShadowDynamicScissor);

    VkDeviceSize directionalShadowOffsets[] = {0};

    // draw/populate the depth map.
    vkCmdBindVertexBuffers(graphicsRecordingPackage.graphicsCommandBuffer, 0, 1, &graphicsRecordingPackage.directionalShadowShaderBufferComponents.vertexBuffer, directionalShadowOffsets);

    vkCmdBindDescriptorSets(graphicsRecordingPackage.graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsRecordingPackage.directionalShadowOperation.pipelineComponents.pipelineLayout, 0, 1, &graphicsRecordingPackage.directionalShadowOperation.pipelineComponents.descriptorSets[graphicsRecordingPackage.currentFrame], 0, nullptr);
    vkCmdBindPipeline(graphicsRecordingPackage.graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsRecordingPackage.directionalShadowOperation.pipelineComponents.pipeline);

    if ((graphicsRecordingPackage.directionalShadowShaderBufferComponents.indiceCount != -1) && (graphicsRecordingPackage.directionalShadowShaderBufferComponents.verticeCount == -1)) {
        vkCmdBindIndexBuffer(graphicsRecordingPackage.graphicsCommandBuffer, graphicsRecordingPackage.directionalShadowShaderBufferComponents.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(graphicsRecordingPackage.graphicsCommandBuffer, graphicsRecordingPackage.directionalShadowShaderBufferComponents.indiceCount, 1, 0, 0, 0);
    } else if ((graphicsRecordingPackage.directionalShadowShaderBufferComponents.indiceCount == -1) && (graphicsRecordingPackage.directionalShadowShaderBufferComponents.verticeCount != -1)) {
        vkCmdDraw(graphicsRecordingPackage.graphicsCommandBuffer, graphicsRecordingPackage.directionalShadowShaderBufferComponents.verticeCount, 1, 0, 0);
    }

    vkCmdEndRenderPass(graphicsRecordingPackage.graphicsCommandBuffer);


    VkRenderPassBeginInfo pointShadowRenderPassBeginInfo{};
    CommandManager::populateRenderPassBeginInfo(graphicsRecordingPackage.pointShadowOperation.renderPass, {}, graphicsRecordingPackage.pointShadowOperation.offscreenExtent, static_cast<uint32_t>(pointShadowAttachmentClearValues.size()), pointShadowAttachmentClearValues.data(), pointShadowRenderPassBeginInfo);

    // set our dynamic pipeline states.
    VkViewport pointShadowDynamicViewport{};
    CommandManager::populateViewportInfo(0.0f, 0.0f, static_cast<float>(graphicsRecordingPackage.pointShadowOperation.offscreenExtent.width), static_cast<float>(graphicsRecordingPackage.pointShadowOperation.offscreenExtent.height), 0.0f, 1.0f, pointShadowDynamicViewport);
    vkCmdSetViewport(graphicsRecordingPackage.graphicsCommandBuffer, 0, 1, &pointShadowDynamicViewport);

    VkRect2D pointShadowDynamicScissor{};
    CommandManager::populateRect2DInfo(graphicsRecordingPackage.pointShadowOperation.offscreenExtent, pointShadowDynamicScissor);
    vkCmdSetScissor(graphicsRecordingPackage.graphicsCommandBuffer, 0, 1, &pointShadowDynamicScissor);

    VkDeviceSize pointShadowOffsets[] = {0};

    for (size_t i = 0; i < 6; i++) {
        uint32_t framebufferIndex = (graphicsRecordingPackage.currentFrame + (Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT * i));
        pointShadowRenderPassBeginInfo.framebuffer = graphicsRecordingPackage.pointShadowOperation.framebuffers[framebufferIndex];
        
        vkCmdBeginRenderPass(graphicsRecordingPackage.graphicsCommandBuffer, &pointShadowRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        

        vkCmdPushConstants(graphicsRecordingPackage.graphicsCommandBuffer, graphicsRecordingPackage.pointShadowOperation.pipelineComponents.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Uniform::PointShadowPushConstants), &Shader::shadowTransforms[i]);

        // draw/populate the depth map.
        vkCmdBindVertexBuffers(graphicsRecordingPackage.graphicsCommandBuffer, 0, 1, &graphicsRecordingPackage.pointShadowShaderBufferComponents.vertexBuffer, pointShadowOffsets);

        vkCmdBindDescriptorSets(graphicsRecordingPackage.graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsRecordingPackage.pointShadowOperation.pipelineComponents.pipelineLayout, 0, 1, &graphicsRecordingPackage.pointShadowOperation.pipelineComponents.descriptorSets[graphicsRecordingPackage.currentFrame], 0, nullptr);
        vkCmdBindPipeline(graphicsRecordingPackage.graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsRecordingPackage.pointShadowOperation.pipelineComponents.pipeline);

        if ((graphicsRecordingPackage.pointShadowShaderBufferComponents.indiceCount != -1) && (graphicsRecordingPackage.pointShadowShaderBufferComponents.verticeCount == -1)) {
            vkCmdBindIndexBuffer(graphicsRecordingPackage.graphicsCommandBuffer, graphicsRecordingPackage.pointShadowShaderBufferComponents.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

            vkCmdDrawIndexed(graphicsRecordingPackage.graphicsCommandBuffer, graphicsRecordingPackage.pointShadowShaderBufferComponents.indiceCount, 1, 0, 0, 0);
        } else if ((graphicsRecordingPackage.pointShadowShaderBufferComponents.indiceCount == -1) && (graphicsRecordingPackage.pointShadowShaderBufferComponents.verticeCount != -1)) {
            vkCmdDraw(graphicsRecordingPackage.graphicsCommandBuffer, graphicsRecordingPackage.pointShadowShaderBufferComponents.verticeCount, 1, 0, 0);
        }

        
        vkCmdEndRenderPass(graphicsRecordingPackage.graphicsCommandBuffer);   
    }
    
    
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
    // index buffer optionally binded later.

    vkCmdBindDescriptorSets(graphicsRecordingPackage.graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsRecordingPackage.scenePipelineComponents.pipelineLayout, 0, 1, &graphicsRecordingPackage.scenePipelineComponents.descriptorSets[graphicsRecordingPackage.currentFrame], 0, nullptr);
    vkCmdBindPipeline(graphicsRecordingPackage.graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsRecordingPackage.scenePipelineComponents.pipeline);

    if ((graphicsRecordingPackage.sceneShaderBufferComponents.indiceCount != -1) && (graphicsRecordingPackage.sceneShaderBufferComponents.verticeCount == -1)) {
        vkCmdBindIndexBuffer(graphicsRecordingPackage.graphicsCommandBuffer, graphicsRecordingPackage.sceneShaderBufferComponents.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(graphicsRecordingPackage.graphicsCommandBuffer, graphicsRecordingPackage.sceneShaderBufferComponents.indiceCount, 1, 0, 0, 0);
    } else if ((graphicsRecordingPackage.sceneShaderBufferComponents.indiceCount == -1) && (graphicsRecordingPackage.sceneShaderBufferComponents.verticeCount != -1)) {
        vkCmdDraw(graphicsRecordingPackage.graphicsCommandBuffer, graphicsRecordingPackage.sceneShaderBufferComponents.verticeCount, 1, 0, 0);
    }

    // draw the scene normals.
    vkCmdBindVertexBuffers(graphicsRecordingPackage.graphicsCommandBuffer, 0, 1, &graphicsRecordingPackage.sceneNormalsShaderBufferComponents.vertexBuffer, mainOffsets);
    // index buffer optionally binded later.
    
    vkCmdBindDescriptorSets(graphicsRecordingPackage.graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsRecordingPackage.sceneNormalsPipelineComponents.pipelineLayout, 0, 1, &graphicsRecordingPackage.sceneNormalsPipelineComponents.descriptorSets[graphicsRecordingPackage.currentFrame], 0, nullptr);
    vkCmdBindPipeline(graphicsRecordingPackage.graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsRecordingPackage.sceneNormalsPipelineComponents.pipeline);

    if ((graphicsRecordingPackage.sceneNormalsShaderBufferComponents.indiceCount != -1) && (graphicsRecordingPackage.sceneNormalsShaderBufferComponents.verticeCount == -1)) {
        vkCmdBindIndexBuffer(graphicsRecordingPackage.graphicsCommandBuffer, graphicsRecordingPackage.sceneNormalsShaderBufferComponents.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(graphicsRecordingPackage.graphicsCommandBuffer, graphicsRecordingPackage.sceneNormalsShaderBufferComponents.indiceCount, 1, 0, 0, 0);
    } else if ((graphicsRecordingPackage.sceneNormalsShaderBufferComponents.indiceCount == -1) && (graphicsRecordingPackage.sceneNormalsShaderBufferComponents.verticeCount != -1)) {
        vkCmdDraw(graphicsRecordingPackage.graphicsCommandBuffer, graphicsRecordingPackage.sceneNormalsShaderBufferComponents.verticeCount, 1, 0, 0);
    }
    
    vkCmdEndRenderPass(graphicsRecordingPackage.graphicsCommandBuffer);
    

    uint32_t recordBufferCommandsResult = vkEndCommandBuffer(graphicsRecordingPackage.graphicsCommandBuffer);
    if (recordBufferCommandsResult != VK_SUCCESS) {
        throwDebugException("Failed to end recording graphics command buffer commands.");
    }
}
