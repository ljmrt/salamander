#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Renderer/Renderer.h>
#include <core/Shader/Shader.h>
#include <core/Shader/ResourceDescriptor.h>
#include <core/Shader/Uniform.h>
#include <core/Shader/Image.h>
#include <core/Shader/Depth.h>
#include <core/Buffer/Buffer.h>
#include <core/DisplayManager/DisplayManager.h>
#include <core/DisplayManager/SwapchainHandler.h>
#include <core/Command/CommandManager.h>
#include <core/VulkanInstance/VulkanInstance.h>
#include <core/Model/ModelHandler.h>
#include <core/Logging/ErrorLogger.h>
#include <core/Defaults/Defaults.h>

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>


void Renderer::populateColorAttachmentComponents(VkFormat swapchainImageFormat, VkSampleCountFlagBits msaaSampleCount, VkAttachmentDescription& colorAttachmentDescription, VkAttachmentReference& colorAttachmentReference, VkAttachmentDescription& colorAttachmentResolveDescription, VkAttachmentReference& colorAttachmentResolveReference)
{
    colorAttachmentDescription.format = swapchainImageFormat;
    colorAttachmentDescription.samples = msaaSampleCount;
    
    colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;  // clears framebuffer to black.
    colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    
    colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


    colorAttachmentResolveDescription.format = swapchainImageFormat;
    colorAttachmentResolveDescription.samples = VK_SAMPLE_COUNT_1_BIT;

    colorAttachmentResolveDescription.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolveDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentResolveDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolveDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    colorAttachmentResolveDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentResolveDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


    colorAttachmentResolveReference.attachment = 2;
    colorAttachmentResolveReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
}

void Renderer::populateDepthAttachmentComponents(VkSampleCountFlagBits msaaSampleCount, VkPhysicalDevice vulkanPhysicalDevice, VkAttachmentDescription& depthAttachmentDescription, VkAttachmentReference& depthAttachmentReference)
{
    VkFormat depthAttachmentDescriptionFormat;
    Depth::selectDepthImageFormat(vulkanPhysicalDevice, depthAttachmentDescriptionFormat);
    depthAttachmentDescription.format = depthAttachmentDescriptionFormat;
    depthAttachmentDescription.samples = msaaSampleCount;

    depthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;  // we have no need for the previous depth buffer's contents.
    depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


    depthAttachmentReference.attachment = 1;
    depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
}

void Renderer::populateSubpassDescription(VkAttachmentReference& colorAttachmentReference, VkAttachmentReference& colorAttachmentResolveReference, VkAttachmentReference& depthAttachmentReference, VkSubpassDescription& subpassDescription)
{
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorAttachmentReference;
    subpassDescription.pResolveAttachments = &colorAttachmentResolveReference;

    subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
}

void Renderer::createMemberRenderPass(VkFormat swapchainImageFormat, VkSampleCountFlagBits msaaSampleCount, VkPhysicalDevice vulkanPhysicalDevice)
{
    VkAttachmentDescription colorAttachmentDescription{};
    VkAttachmentReference colorAttachmentReference{};
    // color attachment resolve "resolves" the general color attachment for presentation.
    VkAttachmentDescription colorAttachmentResolveDescription{};
    VkAttachmentReference colorAttachmentResolveReference{};
    populateColorAttachmentComponents(swapchainImageFormat, msaaSampleCount, colorAttachmentDescription, colorAttachmentReference, colorAttachmentResolveDescription, colorAttachmentResolveReference);


    VkAttachmentDescription depthAttachmentDescription{};
    VkAttachmentReference depthAttachmentReference{};
    populateDepthAttachmentComponents(msaaSampleCount, vulkanPhysicalDevice, depthAttachmentDescription, depthAttachmentReference);
    
    
    VkSubpassDescription subpassDescription{};
    populateSubpassDescription(colorAttachmentReference, colorAttachmentResolveReference, depthAttachmentReference, subpassDescription);

    
    VkSubpassDependency subpassDependency{};
    
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;

    // operations to wait on/operations that should wait.
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;



    std::array<VkAttachmentDescription, 3> attachmentDescriptions = {colorAttachmentDescription, depthAttachmentDescription, colorAttachmentResolveDescription};
    
    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    
    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
    renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
    
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;

    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &subpassDependency;

    
    size_t renderPassCreationResult = vkCreateRenderPass(*m_vulkanLogicalDevice, &renderPassCreateInfo, nullptr, &m_renderPass);
    if (renderPassCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create render pass.");
    }
}

void Renderer::populateViewportCreateInfo(VkPipelineViewportStateCreateInfo& viewportCreateInfo)
{
    viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

    // dynamic viewport and scissor setting.
    viewportCreateInfo.viewportCount = 1;
    viewportCreateInfo.scissorCount = 1;
}

void Renderer::populateRasterizationCreateInfo(VkPipelineRasterizationStateCreateInfo& rasterizationCreateInfo)
{
    rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    
    rasterizationCreateInfo.depthClampEnable = VK_FALSE;  // discard rather than clamp fragments beyond the near and far planes. 
    rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;  // pass through the rasterization stage.
    
    // TODO: wireframe mode(fragments for only the edges of polygons): polygon mode VK_POLYGON_MODE_LINE
    // TODO: point mode(fragments for only the polygon vertices): polygon mode VK_POLYGON_MODE_POINT
    rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;  // fill the area of the polygon with fragments.
    
    rasterizationCreateInfo.lineWidth = 1.0f;
    
    rasterizationCreateInfo.cullMode = VK_CULL_MODE_NONE;
    rasterizationCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;  // vertex order for faces to be considered front-facing, counter-clockwise due to the GLM y-axis flip.
    
    rasterizationCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationCreateInfo.depthBiasConstantFactor = 0.0f;
    rasterizationCreateInfo.depthBiasClamp = 0.0f;
    rasterizationCreateInfo.depthBiasSlopeFactor = 0.0f;
}

void Renderer::fetchMaximumUsableSampleCount(VkPhysicalDevice vulkanPhysicalDevice, VkSampleCountFlagBits& maximumUsableSampleCount)
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(vulkanPhysicalDevice, &physicalDeviceProperties);

    VkSampleCountFlags sampleCountFlags = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;

    // is there no more efficient way to do this?
    if (sampleCountFlags & VK_SAMPLE_COUNT_64_BIT) {
        maximumUsableSampleCount = VK_SAMPLE_COUNT_64_BIT;
        return;
    }
    if (sampleCountFlags & VK_SAMPLE_COUNT_32_BIT) {
        maximumUsableSampleCount = VK_SAMPLE_COUNT_32_BIT;
        return;
    }
    if (sampleCountFlags & VK_SAMPLE_COUNT_16_BIT) {
        maximumUsableSampleCount = VK_SAMPLE_COUNT_16_BIT;
        return;
    }
    if (sampleCountFlags & VK_SAMPLE_COUNT_8_BIT) {
        maximumUsableSampleCount = VK_SAMPLE_COUNT_8_BIT;
        return;
    }
    if (sampleCountFlags & VK_SAMPLE_COUNT_4_BIT) {
        maximumUsableSampleCount = VK_SAMPLE_COUNT_4_BIT;
        return;
    }
    if (sampleCountFlags & VK_SAMPLE_COUNT_2_BIT) {
        maximumUsableSampleCount = VK_SAMPLE_COUNT_2_BIT;
        return;
    }
    maximumUsableSampleCount = VK_SAMPLE_COUNT_1_BIT;
}

void Renderer::populateMultisamplingCreateInfo(VkSampleCountFlagBits msaaSampleCount, VkPipelineMultisampleStateCreateInfo& multisamplingCreateInfo)
{
    multisamplingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    
    multisamplingCreateInfo.sampleShadingEnable = VK_TRUE;
    multisamplingCreateInfo.rasterizationSamples = msaaSampleCount;
    multisamplingCreateInfo.minSampleShading = 0.2f;
    multisamplingCreateInfo.pSampleMask = nullptr;
    
    multisamplingCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multisamplingCreateInfo.alphaToOneEnable = VK_FALSE;
}

void Renderer::populateDepthStencilCreateInfo(VkPipelineDepthStencilStateCreateInfo& depthStencilCreateInfo)
{
    depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    depthStencilCreateInfo.depthTestEnable = VK_TRUE;
    depthStencilCreateInfo.depthWriteEnable = VK_TRUE;

    depthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;  // lower depth means closer object.
    
    depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilCreateInfo.minDepthBounds = 0.0f;
    depthStencilCreateInfo.maxDepthBounds = 1.0f;

    depthStencilCreateInfo.stencilTestEnable = VK_FALSE;
    depthStencilCreateInfo.front = {};
    depthStencilCreateInfo.back = {};
}

void Renderer::populateColorBlendComponents(VkPipelineColorBlendAttachmentState& colorBlendAttachment, VkPipelineColorBlendStateCreateInfo& colorBlendCreateInfo)
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

void Renderer::populateDynamicStatesCreateInfo(std::vector<VkDynamicState>& dynamicStates, VkPipelineDynamicStateCreateInfo& dynamicStatesCreateInfo)
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

void Renderer::createMemberGraphicsPipeline(VkSampleCountFlagBits msaaSampleCount)
{
    std::string vertexBytecodeFilePath = Defaults::miscDefaults.SALAMANDER_ROOT_DIRECTORY + "/build/vertex.spv";
    std::string fragmentBytecodeFilePath = Defaults::miscDefaults.SALAMANDER_ROOT_DIRECTORY + "/build/fragment.spv";

    Shader::createShader(vertexBytecodeFilePath, VK_SHADER_STAGE_VERTEX_BIT, *m_vulkanLogicalDevice, m_pipelineShaders.vertexShader);
    Shader::createShader(fragmentBytecodeFilePath, VK_SHADER_STAGE_FRAGMENT_BIT, *m_vulkanLogicalDevice, m_pipelineShaders.fragmentShader);
    
    VkPipelineShaderStageCreateInfo shaderStageCreateInfos[] = {m_pipelineShaders.vertexShader.shaderStageCreateInfo, m_pipelineShaders.fragmentShader.shaderStageCreateInfo};
    

    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
    ModelHandler::populateVertexInputCreateInfo(vertexInputCreateInfo);


    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
    ModelHandler::populateInputAssemblyCreateInfo(inputAssemblyCreateInfo);


    VkPipelineViewportStateCreateInfo viewportCreateInfo{};
    populateViewportCreateInfo(viewportCreateInfo);

    
    VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo{};
    populateRasterizationCreateInfo(rasterizationCreateInfo);


    // multisampling is currently disabled.
    VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo{};
    populateMultisamplingCreateInfo(msaaSampleCount, multisamplingCreateInfo);


    VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo{};
    populateDepthStencilCreateInfo(depthStencilCreateInfo);


    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo{};
    populateColorBlendComponents(colorBlendAttachment, colorBlendCreateInfo);

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicStatesCreateInfo{};
    populateDynamicStatesCreateInfo(dynamicStates, dynamicStatesCreateInfo);


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
    pipelineCreateInfo.pDepthStencilState = &depthStencilCreateInfo;
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

    if (imageAcquisitionResult == VK_ERROR_OUT_OF_DATE_KHR) {
        SwapchainHandler::recreateSwapchain(DeviceHandler::VulkanDevices{vulkanPhysicalDevice, *m_vulkanLogicalDevice}, m_renderPass, m_graphicsCommandPool, displayDetails.vulkanDisplayDetails.graphicsQueue, displayDetails);
        return;
    } else if (imageAcquisitionResult != VK_SUCCESS && imageAcquisitionResult != VK_SUBOPTIMAL_KHR) {
        throwDebugException("Failed to acquire swapchain image.");
    }
    

    vkResetFences(*m_vulkanLogicalDevice, 1, &m_inFlightFences[m_currentFrame]);  // reset fences only after successful image acquisition.


    vkResetCommandBuffer(m_graphicsCommandBuffers[m_currentFrame], 0);  // 0 for no additional flags.
    CommandManager::recordGraphicsCommandBufferCommands(m_graphicsCommandBuffers[m_currentFrame], m_renderPass, displayDetails.vulkanDisplayDetails.swapchainFramebuffers[swapchainImageIndex], displayDetails.vulkanDisplayDetails.swapchainImageExtent, m_graphicsPipeline, m_mainModel.vertexBuffer, m_mainModel.indexBuffer, m_pipelineLayout, m_descriptorSets, m_currentFrame, static_cast<uint32_t>(m_mainModel.meshIndices.size()));

    
    Uniform::updateFrameUniformBuffer(m_mainCamera, m_mainModel.meshQuaternion, m_currentFrame, displayDetails.glfwWindow, displayDetails.vulkanDisplayDetails.swapchainImageExtent, m_mappedUniformBuffersMemory);

    
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
    DeviceHandler::VulkanDevices temporaryVulkanDevices{};
    temporaryVulkanDevices.physicalDevice = vulkanPhysicalDevice;
    temporaryVulkanDevices.logicalDevice = *m_vulkanLogicalDevice;


    Defaults::callbacksVariables.MAIN_CAMERA = &m_mainCamera;
    m_mainCamera.up = glm::vec3(0.0f, 1.0f, 0.0f);
    m_mainCamera.eye = glm::vec3(0.0f, 0.0f, -3.0f);
    m_mainCamera.center = glm::vec3(0.0f, 0.0f, 0.0f);
    m_mainCamera.swapchainImageExtent = displayDetails.vulkanDisplayDetails.swapchainImageExtent;

    fetchMaximumUsableSampleCount(vulkanPhysicalDevice, displayDetails.vulkanDisplayDetails.msaaSampleCount);
    
    createMemberRenderPass(displayDetails.vulkanDisplayDetails.swapchainImageFormat, displayDetails.vulkanDisplayDetails.msaaSampleCount, vulkanPhysicalDevice);

    ResourceDescriptor::createDescriptorSetLayout(*m_vulkanLogicalDevice, m_descriptorSetLayout);
        
    createMemberGraphicsPipeline(displayDetails.vulkanDisplayDetails.msaaSampleCount);

    CommandManager::createGraphicsCommandPool(graphicsFamilyIndex, *m_vulkanLogicalDevice, m_graphicsCommandPool);

    // populate the color image details.
    Image::populateImageDetails(displayDetails.vulkanDisplayDetails.swapchainImageExtent.width, displayDetails.vulkanDisplayDetails.swapchainImageExtent.height, 1, displayDetails.vulkanDisplayDetails.msaaSampleCount, displayDetails.vulkanDisplayDetails.swapchainImageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, temporaryVulkanDevices, displayDetails.vulkanDisplayDetails.colorImageDetails);
    Image::createImageView(displayDetails.vulkanDisplayDetails.colorImageDetails.image, displayDetails.vulkanDisplayDetails.colorImageDetails.imageFormat, 1, VK_IMAGE_ASPECT_COLOR_BIT, *m_vulkanLogicalDevice, displayDetails.vulkanDisplayDetails.colorImageDetails.imageView);
    
    Depth::populateDepthImageDetails(displayDetails.vulkanDisplayDetails.swapchainImageExtent, displayDetails.vulkanDisplayDetails.msaaSampleCount, m_graphicsCommandPool, displayDetails.vulkanDisplayDetails.graphicsQueue, temporaryVulkanDevices, displayDetails.vulkanDisplayDetails.depthImageDetails);

    SwapchainHandler::createSwapchainFramebuffers(displayDetails.vulkanDisplayDetails.swapchainImageViews, displayDetails.vulkanDisplayDetails.colorImageDetails.imageView, displayDetails.vulkanDisplayDetails.depthImageDetails.imageView, m_renderPass, displayDetails.vulkanDisplayDetails.swapchainImageExtent, *m_vulkanLogicalDevice, displayDetails.vulkanDisplayDetails.swapchainFramebuffers);  // in render function due to timing of swapchain framebuffer creation.

    m_mainModel.loadModelFromAbsolutePath((Defaults::miscDefaults.SALAMANDER_ROOT_DIRECTORY + "/assets/models/Avocado/Avocado.gltf"));
    // TODO: add seperate "transfer" queue(see vulkan-tutorial page).
    m_mainModel.createModelBuffers(m_graphicsCommandPool, displayDetails.vulkanDisplayDetails.graphicsQueue, temporaryVulkanDevices);
    Image::populateTextureDetails(m_mainModel.absoluteTextureImagePath, m_graphicsCommandPool, displayDetails.vulkanDisplayDetails.graphicsQueue, temporaryVulkanDevices, m_mainModel.textureDetails);

    Uniform::createUniformBuffers(temporaryVulkanDevices, m_uniformBuffers, m_uniformBuffersMemory, m_mappedUniformBuffersMemory);
    
    ResourceDescriptor::createDescriptorPool(*m_vulkanLogicalDevice, m_descriptorPool);
    ResourceDescriptor::createDescriptorSets(m_descriptorSetLayout, m_descriptorPool, *m_vulkanLogicalDevice, m_descriptorSets);
    ResourceDescriptor::populateDescriptorSets(m_uniformBuffers, m_mainModel.textureDetails.textureImageDetails.imageView, m_mainModel.textureDetails.textureSampler, *m_vulkanLogicalDevice, m_descriptorSets);

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
    m_mainModel.cleanupModel(*m_vulkanLogicalDevice);
    
    for (size_t i = 0; i < Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT; i += 1) {
        vkDestroySemaphore(*m_vulkanLogicalDevice, m_imageAvailibleSemaphores[i], nullptr);
        vkDestroySemaphore(*m_vulkanLogicalDevice, m_renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(*m_vulkanLogicalDevice, m_inFlightFences[i], nullptr);

        vkDestroyBuffer(*m_vulkanLogicalDevice, m_uniformBuffers[i], nullptr);
        vkFreeMemory(*m_vulkanLogicalDevice, m_uniformBuffersMemory[i], nullptr);
    }

    vkDestroyCommandPool(*m_vulkanLogicalDevice, m_graphicsCommandPool, nullptr);  // child command buffers automatically freed.

    vkDestroyDescriptorPool(*m_vulkanLogicalDevice, m_descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(*m_vulkanLogicalDevice, m_descriptorSetLayout, nullptr);
    
    vkDestroyPipeline(*m_vulkanLogicalDevice, m_graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(*m_vulkanLogicalDevice, m_pipelineLayout, nullptr);
    vkDestroyRenderPass(*m_vulkanLogicalDevice, m_renderPass, nullptr);
}

Renderer::Renderer()
{
    
}
