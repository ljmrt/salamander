#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Renderer/Renderer.h>
#include <core/Shader/Shader.h>
#include <core/DisplayManager/DisplayManager.h>
#include <core/DisplayManager/swapchainHandler.h>
#include <core/Command/CommandManager.h>
#include <core/VulkanInstance/VulkanInstance.h>
#include <core/Model/vertexHandler.h>
#include <core/Logging/ErrorLogger.h>
#include <core/Defaults/Defaults.h>

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>


void Renderer::fillColorAttachment(VkFormat swapchainImageFormat, VkAttachmentDescription& colorAttachmentDescription, VkAttachmentReference& colorAttachmentReference)
{
    colorAttachmentDescription.format = swapchainImageFormat;
    colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    
    colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;  // clear framebuffer to black before renderering.
    colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;  // ignore stencil buffer loading and storing.
    colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    
    colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;  // transition image into swapchain-ready format.

    
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
}

void Renderer::fillSubpassDescription(VkAttachmentReference *colorAttachmentReference, VkSubpassDescription& subpassDescription)
{
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = colorAttachmentReference;
}

void Renderer::createMemberRenderPass(VkFormat swapchainImageFormat)
{
    VkAttachmentDescription colorAttachmentDescription{};
    VkAttachmentReference colorAttachmentReference{};
    fillColorAttachment(swapchainImageFormat, colorAttachmentDescription, colorAttachmentReference);
    
    
    VkSubpassDescription subpassDescription{};
    fillSubpassDescription(&colorAttachmentReference, subpassDescription);
    
    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = &colorAttachmentDescription;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;

    
    VkSubpassDependency subpassDependency{};
    
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;

    // operations to wait on.
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;  // wait for the swapchain to finish reading the image before accessing.
    subpassDependency.srcAccessMask = 0;

    // operations that should wait.
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &subpassDependency;
    
    size_t renderPassCreationResult = vkCreateRenderPass(*m_vulkanLogicalDevice, &renderPassCreateInfo, nullptr, &m_renderPass);
    if (renderPassCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create render pass.");
    }
}

void Renderer::createMemberDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding uniformBufferLayoutBinding{};
    
    uniformBufferLayoutBinding.binding = 0;  // specified in the vertex shader.
    
    uniformBufferLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformBufferLayoutBinding.descriptorCount = 1;

    uniformBufferLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;  // descriptor only referenced in the vertex shader.

    uniformBufferLayoutBinding.pImmutableSamplers = nullptr;


    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    
    descriptorSetLayoutCreateInfo.bindingCount = 1;
    descriptorSetLayoutCreateInfo.pBindings = &uniformBufferLayoutBinding;

    VkResult descriptorSetLayoutCreationResult = vkCreateDescriptorSetLayout(*m_vulkanLogicalDevice, &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayout);
    if (descriptorSetLayoutCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create descriptor set layout.");
    }
}

void Renderer::createMemberDescriptorPool()
{
    VkDescriptorPoolSize descriptorPoolSize{};
    descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    
    descriptorPoolSize.descriptorCount = static_cast<uint32_t>(Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT);


    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

    descriptorPoolCreateInfo.poolSizeCount = 1;
    descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;

    descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT);

    VkResult descriptorPoolCreationResult = vkCreateDescriptorPool(*m_vulkanLogicalDevice, &descriptorPoolCreateInfo, nullptr, &m_descriptorPool);
    if (descriptorPoolCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create descriptor pool.");
    }
}

void Renderer::createMemberDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts(Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT, m_descriptorSetLayout);
    
    VkDescriptorSetAllocateInfo descriptorSetsAllocateInfo{};
    descriptorSetsAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

    descriptorSetsAllocateInfo.descriptorPool = m_descriptorPool;
    
    descriptorSetsAllocateInfo.descriptorSetCount = static_cast<uint32_t>(Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT);
    descriptorSetsAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

    m_descriptorSets.resize(Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT);
    VkResult descriptorSetsAllocationResult = vkAllocateDescriptorSets(*m_vulkanLogicalDevice, &descriptorSetsAllocateInfo, m_descriptorSets.data());
    if (descriptorSetsAllocationResult != VK_SUCCESS) {
        throwDebugException("Failed to allocate descriptor sets.");
    }
}

void Renderer::populateMemberDescriptorSets()
{
    for (size_t i = 0; i < m_descriptorSets.size(); i += 1) {
        VkDescriptorBufferInfo descriptorBufferInfo{};

        descriptorBufferInfo.buffer = m_uniformBuffers[i];
        
        descriptorBufferInfo.offset = 0;
        descriptorBufferInfo.range = VK_WHOLE_SIZE;  // we're overwriting the entire buffer.


        VkWriteDescriptorSet writeDescriptorSet{};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

        writeDescriptorSet.dstSet = m_descriptorSets[i];
        writeDescriptorSet.dstBinding = 0;  // the uniform buffer binding index.
        writeDescriptorSet.dstArrayElement = 0;  // we're not using an array.

        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;  // we're using this for our uniform buffer.
        writeDescriptorSet.descriptorCount = 1;

        writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;
        writeDescriptorSet.pImageInfo = nullptr;
        writeDescriptorSet.pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(*m_vulkanLogicalDevice, 1, &writeDescriptorSet, 0, nullptr);  // populate/update the descriptor set.
    }
}

void Renderer::fillVertexInputCreateInfo(VkPipelineVertexInputStateCreateInfo& vertexInputCreateInfo)
{
    vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    vertexHandler::fetchBindingDescription(m_bindingDescription);

    vertexHandler::fetchAttributeDescriptions(m_attributeDescriptions);
    
    vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputCreateInfo.pVertexBindingDescriptions = &m_bindingDescription;
    vertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_attributeDescriptions.size());
    vertexInputCreateInfo.pVertexAttributeDescriptions = m_attributeDescriptions.data();
}

void Renderer::fillInputAssemblyCreateInfo(VkPipelineInputAssemblyStateCreateInfo& inputAssemblyCreateInfo)
{
    inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    
    inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;  // form triangle primitives without vertex reuse.
    inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;  // disable the possibility of vertex reuse.
}

void Renderer::fillViewportCreateInfo(VkPipelineViewportStateCreateInfo& viewportCreateInfo)
{
    viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

    // dynamic viewport and scissor setting.
    viewportCreateInfo.viewportCount = 1;
    viewportCreateInfo.scissorCount = 1;
}

void Renderer::fillRasterizationCreateInfo(VkPipelineRasterizationStateCreateInfo& rasterizationCreateInfo)
{
    rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    
    rasterizationCreateInfo.depthClampEnable = VK_FALSE;  // discard rather than clamp fragments beyond the near and far planes. 
    rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;  // pass through the rasterization stage.
    
    // TODO: wireframe mode(fragments for only the edges of polygons): polygon mode VK_POLYGON_MODE_LINE
    // TODO: point mode(fragments for only the polygon vertices): polygon mode VK_POLYGON_MODE_POINT
    rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;  // fill the area of the polygon with fragments.
    
    rasterizationCreateInfo.lineWidth = 1.0f;
    
    rasterizationCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;  // vertex order for faces to be considered front-facing, counter-clockwise due to the GLM y-axis flip.
    
    rasterizationCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationCreateInfo.depthBiasConstantFactor = 0.0f;
    rasterizationCreateInfo.depthBiasClamp = 0.0f;
    rasterizationCreateInfo.depthBiasSlopeFactor = 0.0f;
}

void Renderer::fillMultisamplingCreateInfo(VkPipelineMultisampleStateCreateInfo& multisamplingCreateInfo)
{
    multisamplingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    
    multisamplingCreateInfo.sampleShadingEnable = VK_FALSE;
    multisamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisamplingCreateInfo.minSampleShading = 1.0f;
    multisamplingCreateInfo.pSampleMask = nullptr;
    
    multisamplingCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multisamplingCreateInfo.alphaToOneEnable = VK_FALSE;
}

void Renderer::fillColorBlend(VkPipelineColorBlendAttachmentState& colorBlendAttachment, VkPipelineColorBlendStateCreateInfo& colorBlendCreateInfo)
{
    // "local" pipeline-specific color blending(hence the name attachment).
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    // "global" application-specific color blending.
    colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
    colorBlendCreateInfo.attachmentCount = 1;
    colorBlendCreateInfo.pAttachments = &colorBlendAttachment;
    colorBlendCreateInfo.blendConstants[0] = 0.0f;
    colorBlendCreateInfo.blendConstants[1] = 0.0f;
    colorBlendCreateInfo.blendConstants[2] = 0.0f;
    colorBlendCreateInfo.blendConstants[3] = 0.0f;
}

void Renderer::fillDynamicStatesCreateInfo(std::vector<VkDynamicState>& dynamicStates, VkPipelineDynamicStateCreateInfo& dynamicStatesCreateInfo)
{
    dynamicStatesCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    
    dynamicStatesCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicStatesCreateInfo.pDynamicStates = dynamicStates.data();    
}

void Renderer::createMemberPipelineLayout()
{
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.flags = 0;
    
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &m_descriptorSetLayout;
    
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

    size_t pipelineLayoutCreationResult = vkCreatePipelineLayout(*m_vulkanLogicalDevice, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout);
    if (pipelineLayoutCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create pipeline layout.");
    }
}

void Renderer::createMemberGraphicsPipeline()
{
    std::string vertexBytecodeFilePath = Defaults::miscDefaults.SALAMANDER_ROOT_DIRECTORY + "/build/vertex.spv";
    std::string fragmentBytecodeFilePath = Defaults::miscDefaults.SALAMANDER_ROOT_DIRECTORY + "/build/fragment.spv";

    Shader::createShader(vertexBytecodeFilePath, VK_SHADER_STAGE_VERTEX_BIT, *m_vulkanLogicalDevice, m_pipelineShaders.vertexShader);
    Shader::createShader(fragmentBytecodeFilePath, VK_SHADER_STAGE_FRAGMENT_BIT, *m_vulkanLogicalDevice, m_pipelineShaders.fragmentShader);
    
    VkPipelineShaderStageCreateInfo shaderStageCreateInfos[] = {m_pipelineShaders.vertexShader.shaderStageCreateInfo, m_pipelineShaders.fragmentShader.shaderStageCreateInfo};
    

    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
    fillVertexInputCreateInfo(vertexInputCreateInfo);


    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
    fillInputAssemblyCreateInfo(inputAssemblyCreateInfo);


    VkPipelineViewportStateCreateInfo viewportCreateInfo{};
    fillViewportCreateInfo(viewportCreateInfo);

    
    VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo{};
    fillRasterizationCreateInfo(rasterizationCreateInfo);


    // multisampling is currently disabled.
    VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo{};
    fillMultisamplingCreateInfo(multisamplingCreateInfo);


    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo{};
    fillColorBlend(colorBlendAttachment, colorBlendCreateInfo);

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicStatesCreateInfo{};
    fillDynamicStatesCreateInfo(dynamicStates, dynamicStatesCreateInfo);


    createMemberPipelineLayout();


    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pStages = shaderStageCreateInfos;
    
    pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
    pipelineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
    pipelineCreateInfo.pDepthStencilState = nullptr;
    pipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
    pipelineCreateInfo.pDynamicState = &dynamicStatesCreateInfo;

    pipelineCreateInfo.layout = m_pipelineLayout;
    pipelineCreateInfo.renderPass = m_renderPass;
    pipelineCreateInfo.subpass = 0;

    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = -1;

    size_t pipelineCreationResult = vkCreateGraphicsPipelines(*m_vulkanLogicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_graphicsPipeline);
    if (pipelineCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create graphics pipeline.");
    }


    vkDestroyShaderModule(*m_vulkanLogicalDevice, m_pipelineShaders.fragmentShader.shaderModule, nullptr);
    vkDestroyShaderModule(*m_vulkanLogicalDevice, m_pipelineShaders.vertexShader.shaderModule, nullptr);
}

void Renderer::createMemberSynchronizationObjects()
{
    m_imageAvailibleSemaphores.resize(Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.resize(Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.resize(Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT);
    
    
    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;


    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;  // create the fence signaled to avoid blockage on first run of drawFrame.

    for (size_t i = 0; i < Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT; i += 1) {
        uint32_t imageAvailibleSemaphoreCreationResult = vkCreateSemaphore(*m_vulkanLogicalDevice, &semaphoreCreateInfo, nullptr, &m_imageAvailibleSemaphores[i]);
        uint32_t renderFinishedSemaphoreCreationResult = vkCreateSemaphore(*m_vulkanLogicalDevice, &semaphoreCreateInfo, nullptr, &m_renderFinishedSemaphores[i]);
        uint32_t inFlightFenceCreationResult = vkCreateFence(*m_vulkanLogicalDevice, &fenceCreateInfo, nullptr, &m_inFlightFences[i]);

        if (imageAvailibleSemaphoreCreationResult != VK_SUCCESS || renderFinishedSemaphoreCreationResult != VK_SUCCESS) {
            throwDebugException("Failed to create semaphores.");
        }
        if (inFlightFenceCreationResult != VK_SUCCESS) {
            throwDebugException("Failed to create fence.");
        }
    }
}

void Renderer::drawFrame(DisplayManager::DisplayDetails& displayDetails, VkPhysicalDevice vulkanPhysicalDevice, VkQueue graphicsQueue, VkQueue presentationQueue)
{
    vkWaitForFences(*m_vulkanLogicalDevice, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);  // wait for the GPU to finish with the previous frame, UINT64_MAX timeout.


    uint32_t swapchainImageIndex;  // prefer to use size_t, but want to avoid weird casts to uint32_t.
    VkResult imageAcquisitionResult = vkAcquireNextImageKHR(*m_vulkanLogicalDevice, displayDetails.vulkanDisplayDetails.swapchain, UINT64_MAX, m_imageAvailibleSemaphores[m_currentFrame], VK_NULL_HANDLE, &swapchainImageIndex);  // get the index of an availbile swapchain image.

    // commented out parts are non-functional code checking framebuffer resize.
    if (imageAcquisitionResult == VK_ERROR_OUT_OF_DATE_KHR) {
 // if (imageAcquisitionResult == VK_ERROR_OUT_OF_DATE_KHR || imageAcquisitionResult == VK_SUBOPTIMAL_KHR || Defaults::callbacksVariables.FRAMEBUFFER_RESIZED) {
        // Defaults::callbacksVariables.FRAMEBUFFER_RESIZED = false;
        swapchainHandler::recreateSwapchain(deviceHandler::VulkanDevices{vulkanPhysicalDevice, *m_vulkanLogicalDevice}, m_renderPass, displayDetails);
        return;
    } else if (imageAcquisitionResult != VK_SUCCESS && imageAcquisitionResult != VK_SUBOPTIMAL_KHR) {
 // } else if (imageAcquisitionResult != VK_SUCCESS) {
        throwDebugException("Failed to acquire swapchain image.");
    }
    

    vkResetFences(*m_vulkanLogicalDevice, 1, &m_inFlightFences[m_currentFrame]);  // reset fences only after successful image acquisition.


    vkResetCommandBuffer(m_graphicsCommandBuffers[m_currentFrame], 0);  // 0 for no additional flags.
    CommandManager::recordGraphicsCommandBufferCommands(m_graphicsCommandBuffers[m_currentFrame], m_renderPass, displayDetails.vulkanDisplayDetails.swapchainFramebuffers[swapchainImageIndex], displayDetails.vulkanDisplayDetails.swapchainImageExtent, m_graphicsPipeline, m_vertexBuffer, m_indexBuffer, m_pipelineLayout, m_descriptorSets, m_currentFrame);

    
    vertexHandler::updateUniformBuffer(m_currentFrame, displayDetails.vulkanDisplayDetails.swapchainImageExtent, m_mappedUniformBuffersMemory);

    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // what semaphore to wait on and what stage of the pipeline to wait on.
    VkSemaphore waitSemaphores[] = {m_imageAvailibleSemaphores[m_currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_graphicsCommandBuffers[m_currentFrame];

    // what semaphore to signal when the command buffers have finished executing.
    VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphores[m_currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    uint32_t graphicsQueueSubmitResult = vkQueueSubmit(graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame]);
    if (graphicsQueueSubmitResult != VK_SUCCESS) {
        throwDebugException("Failed to submit graphics command buffer to graphics queue.");
    }


    VkPresentInfoKHR presentationInfo{};
    presentationInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentationInfo.waitSemaphoreCount = 1;
    presentationInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapchains[] = {displayDetails.vulkanDisplayDetails.swapchain};
    presentationInfo.swapchainCount = 1;
    presentationInfo.pSwapchains = swapchains;
    presentationInfo.pImageIndices = &swapchainImageIndex;
    presentationInfo.pResults = nullptr;

    vkQueuePresentKHR(presentationQueue, &presentationInfo);

    
    m_currentFrame = (m_currentFrame + 1) % Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT;  // modulo operator to ensure frame index resets after MAX_FRAMES_IN_FLIGHT frames.
}

void Renderer::setVulkanLogicalDevice(VkDevice *vulkanLogicalDevice)
{
    m_vulkanLogicalDevice = vulkanLogicalDevice;
}

void Renderer::render(DisplayManager::DisplayDetails& displayDetails, size_t graphicsFamilyIndex, VkPhysicalDevice vulkanPhysicalDevice)
{
    deviceHandler::VulkanDevices temporaryVulkanDevices{};
    temporaryVulkanDevices.physicalDevice = vulkanPhysicalDevice;
    temporaryVulkanDevices.logicalDevice = *m_vulkanLogicalDevice;
    
    
    createMemberRenderPass(displayDetails.vulkanDisplayDetails.swapchainImageFormat);

    createMemberDescriptorSetLayout();
        
    createMemberGraphicsPipeline();

    swapchainHandler::createSwapchainFramebuffers(displayDetails.vulkanDisplayDetails.swapchainImageViews, m_renderPass, displayDetails.vulkanDisplayDetails.swapchainImageExtent, *m_vulkanLogicalDevice, displayDetails.vulkanDisplayDetails.swapchainFramebuffers);  // in render function due to timing of swapchain framebuffer creation.

    CommandManager::createGraphicsCommandPool(graphicsFamilyIndex, *m_vulkanLogicalDevice, m_graphicsCommandPool);
    
    // TODO: add seperate "transfer" queue(see vulkan-tutorial page).
    vertexHandler::createDataBufferComponents(vertexHandler::vertices.data(), sizeof(vertexHandler::vertices[0]) * vertexHandler::vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, m_graphicsCommandPool, displayDetails.vulkanDisplayDetails.graphicsQueue, temporaryVulkanDevices, m_vertexBuffer, m_vertexBufferMemory);
    vertexHandler::createDataBufferComponents(vertexHandler::indices.data(), sizeof(vertexHandler::indices[0]) * vertexHandler::indices.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, m_graphicsCommandPool, displayDetails.vulkanDisplayDetails.graphicsQueue, temporaryVulkanDevices, m_indexBuffer, m_indexBufferMemory);

    vertexHandler::createUniformBuffers(temporaryVulkanDevices, m_uniformBuffers, m_uniformBuffersMemory, m_mappedUniformBuffersMemory);
    
    createMemberDescriptorPool();
    createMemberDescriptorSets();
    populateMemberDescriptorSets();

    CommandManager::allocateChildCommandBuffers(m_graphicsCommandPool, Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT, *m_vulkanLogicalDevice, m_graphicsCommandBuffers);

    createMemberSynchronizationObjects();


    while (!glfwWindowShouldClose(displayDetails.glfwWindow)) {  // "main loop"
        DisplayManager::processWindowInput(displayDetails.glfwWindow);
        glfwPollEvents();

        drawFrame(displayDetails, vulkanPhysicalDevice, displayDetails.vulkanDisplayDetails.graphicsQueue, displayDetails.vulkanDisplayDetails.presentationQueue);
    }

    vkDeviceWaitIdle(*m_vulkanLogicalDevice);  // wait for the logical device to finish all operations before termination.
}

void Renderer::cleanupRenderer()
{
    for (size_t i = 0; i < Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT; i += 1) {
        vkDestroySemaphore(*m_vulkanLogicalDevice, m_imageAvailibleSemaphores[i], nullptr);
        vkDestroySemaphore(*m_vulkanLogicalDevice, m_renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(*m_vulkanLogicalDevice, m_inFlightFences[i], nullptr);

        vkDestroyBuffer(*m_vulkanLogicalDevice, m_uniformBuffers[i], nullptr);
        vkFreeMemory(*m_vulkanLogicalDevice, m_uniformBuffersMemory[i], nullptr);
    }
    
    vkDestroyCommandPool(*m_vulkanLogicalDevice, m_graphicsCommandPool, nullptr);  // child command buffers automatically freed.

    vkDestroyBuffer(*m_vulkanLogicalDevice, m_vertexBuffer, nullptr);
    vkFreeMemory(*m_vulkanLogicalDevice, m_vertexBufferMemory, nullptr);

    vkDestroyBuffer(*m_vulkanLogicalDevice, m_indexBuffer, nullptr);
    vkFreeMemory(*m_vulkanLogicalDevice, m_indexBufferMemory, nullptr);

    vkDestroyDescriptorPool(*m_vulkanLogicalDevice, m_descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(*m_vulkanLogicalDevice, m_descriptorSetLayout, nullptr);
    
    vkDestroyPipeline(*m_vulkanLogicalDevice, m_graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(*m_vulkanLogicalDevice, m_pipelineLayout, nullptr);
    vkDestroyRenderPass(*m_vulkanLogicalDevice, m_renderPass, nullptr);
}

Renderer::Renderer()
{
    
}
