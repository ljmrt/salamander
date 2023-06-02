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


void PipelineComponents::cleanupPipelineComponents(VkDevice vulkanLogicalDevice)
{
    for (size_t i = 0; i < Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT; i += 1) {  // uniform buffers are the size of MAX_FRAME_IN_FLIGHT.
        vkDestroyBuffer(vulkanLogicalDevice, uniformBuffers[i], nullptr);
        vkFreeMemory(vulkanLogicalDevice, uniformBuffersMemory[i], nullptr);        
    }
    
    vkDestroyDescriptorPool(vulkanLogicalDevice, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(vulkanLogicalDevice, descriptorSetLayout, nullptr);
    
    vkDestroyPipeline(vulkanLogicalDevice, pipeline, nullptr);
    vkDestroyPipelineLayout(vulkanLogicalDevice, pipelineLayout, nullptr);

    vkDestroyRenderPass(vulkanLogicalDevice, renderPass, nullptr);
}

void Renderer::populateSceneColorAttachmentComponents(VkFormat swapchainImageFormat, VkSampleCountFlagBits msaaSampleCount, VkAttachmentDescription& colorAttachmentDescription, VkAttachmentReference& colorAttachmentReference, VkAttachmentDescription& colorAttachmentResolveDescription, VkAttachmentReference& colorAttachmentResolveReference)
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

void Renderer::populateCubemapColorAttachmentComponents(VkFormat swapchainImageFormat, VkAttachmentDescription& colorAttachmentDescription, VkAttachmentReference& colorAttachmentReference)
{
    colorAttachmentDescription.format = swapchainImageFormat;
    colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    
    colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;  // clears framebuffer to black.
    colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    
    colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
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

void Renderer::populateSubpassDescription(VkAttachmentReference& colorAttachmentReference, bool isCubemap, VkAttachmentReference& colorAttachmentResolveReference, VkAttachmentReference& depthAttachmentReference, VkSubpassDescription& subpassDescription)
{
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorAttachmentReference;

    if (isCubemap == false) {
        subpassDescription.pResolveAttachments = &colorAttachmentResolveReference;

        subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
    }
}

void Renderer::createMemberSceneRenderPass(VkFormat swapchainImageFormat, VkSampleCountFlagBits msaaSampleCount, VkPhysicalDevice vulkanPhysicalDevice)
{
    VkAttachmentDescription colorAttachmentDescription{};
    VkAttachmentReference colorAttachmentReference{};
    // color attachment resolve "resolves" the general color attachment for presentation.
    VkAttachmentDescription colorAttachmentResolveDescription{};
    VkAttachmentReference colorAttachmentResolveReference{};
    populateSceneColorAttachmentComponents(swapchainImageFormat, msaaSampleCount, colorAttachmentDescription, colorAttachmentReference, colorAttachmentResolveDescription, colorAttachmentResolveReference);


    VkAttachmentDescription depthAttachmentDescription{};
    VkAttachmentReference depthAttachmentReference{};
    populateDepthAttachmentComponents(msaaSampleCount, vulkanPhysicalDevice, depthAttachmentDescription, depthAttachmentReference);
    
    
    VkSubpassDescription subpassDescription{};
    populateSubpassDescription(colorAttachmentReference, false, colorAttachmentResolveReference, depthAttachmentReference, subpassDescription);

    
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

    
    size_t renderPassCreationResult = vkCreateRenderPass(*m_vulkanLogicalDevice, &renderPassCreateInfo, nullptr, &m_scenePipelineComponents.renderPass);
    if (renderPassCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create scene render pass.");
    }
}

void Renderer::createMemberCubemapRenderPass(VkFormat swapchainImageFormat, VkPhysicalDevice vulkanPhysicalDevice)
{
    VkAttachmentDescription colorAttachmentDescription{};
    VkAttachmentReference colorAttachmentReference{};
    populateCubemapColorAttachmentComponents(swapchainImageFormat, colorAttachmentDescription, colorAttachmentReference);

    VkAttachmentReference dummyAttachmentReference{};

    VkSubpassDescription subpassDescription{};
    populateSubpassDescription(colorAttachmentReference, true, dummyAttachmentReference, dummyAttachmentReference, subpassDescription);

    
    VkSubpassDependency subpassDependency{};
    
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;

    // operations to wait on/operations that should wait.
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;



    std::array<VkAttachmentDescription, 1> attachmentDescriptions = {colorAttachmentDescription};
    
    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    
    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
    renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
    
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;

    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &subpassDependency;

    
    size_t renderPassCreationResult = vkCreateRenderPass(*m_vulkanLogicalDevice, &renderPassCreateInfo, nullptr, &m_cubemapPipelineComponents.renderPass);
    if (renderPassCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create cubemap render pass.");
    }
}

void Renderer::populateViewportCreateInfo(uint32_t viewportCount, uint32_t scissorCount, VkPipelineViewportStateCreateInfo& viewportCreateInfo)
{
    viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

    // dynamic viewport and scissor setting.
    viewportCreateInfo.viewportCount = viewportCount;
    viewportCreateInfo.scissorCount = scissorCount;
}

void Renderer::populateRasterizationCreateInfo(VkCullModeFlags cullMode, VkFrontFace frontFace, VkPipelineRasterizationStateCreateInfo& rasterizationCreateInfo)
{
    rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    
    rasterizationCreateInfo.depthClampEnable = VK_FALSE;  // default to discarding rather than clamping fragments beyond the near and far planes. 
    rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;  // pass through the rasterization stage.
    
    // TODO: wireframe mode(fragments for only the edges of polygons): polygon mode VK_POLYGON_MODE_LINE
    // TODO: point mode(fragments for only the polygon vertices): polygon mode VK_POLYGON_MODE_POINT
    rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;  // fill the area of the polygon with fragments.
    
    rasterizationCreateInfo.lineWidth = 1.0f;
    
    rasterizationCreateInfo.cullMode = cullMode;
    rasterizationCreateInfo.frontFace = frontFace;
    
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

void Renderer::populateMultisamplingCreateInfo(VkSampleCountFlagBits rasterizationSamples, float minSampleShading, VkPipelineMultisampleStateCreateInfo& multisamplingCreateInfo)
{
    multisamplingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    
    multisamplingCreateInfo.sampleShadingEnable = VK_TRUE;
    multisamplingCreateInfo.rasterizationSamples = rasterizationSamples;
    multisamplingCreateInfo.minSampleShading = minSampleShading;
    multisamplingCreateInfo.pSampleMask = nullptr;
    
    multisamplingCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multisamplingCreateInfo.alphaToOneEnable = VK_FALSE;
}

void Renderer::populateDepthStencilCreateInfo(VkBool32 depthTestEnable, VkBool32 depthWriteEnable, VkCompareOp depthCompareOp, VkPipelineDepthStencilStateCreateInfo& depthStencilCreateInfo)
{
    depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    depthStencilCreateInfo.depthTestEnable = depthTestEnable;
    depthStencilCreateInfo.depthWriteEnable = depthWriteEnable;

    depthStencilCreateInfo.depthCompareOp = depthCompareOp;
    
    depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilCreateInfo.minDepthBounds = 0.0f;
    depthStencilCreateInfo.maxDepthBounds = 1.0f;

    depthStencilCreateInfo.stencilTestEnable = VK_FALSE;
    depthStencilCreateInfo.front = {};
    depthStencilCreateInfo.back = {};
}

void Renderer::populateColorBlendComponents(VkColorComponentFlags colorWriteMask, VkBool32 blendEnable, VkPipelineColorBlendAttachmentState& colorBlendAttachment, VkPipelineColorBlendStateCreateInfo& colorBlendCreateInfo)
{
    // "local" pipeline-specific color blending(hence the name attachment).
    colorBlendAttachment.colorWriteMask = colorWriteMask;
    colorBlendAttachment.blendEnable = blendEnable;
    
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

void Renderer::createMemberPipelineLayout(VkDescriptorSetLayout& descriptorSetLayout, VkPipelineLayout& memberPipelineLayout)
{
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.flags = 0;
    
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
    
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

    VkResult pipelineLayoutCreationResult = vkCreatePipelineLayout(*m_vulkanLogicalDevice, &pipelineLayoutCreateInfo, nullptr, &memberPipelineLayout);
    if (pipelineLayoutCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create member pipeline layout.");
    }
}

void Renderer::createMemberScenePipeline(VkSampleCountFlagBits msaaSampleCount)
{
    std::string vertexBytecodeFilePath = Defaults::miscDefaults.SALAMANDER_ROOT_DIRECTORY + "/build/sceneVertex.spv";
    std::string fragmentBytecodeFilePath = Defaults::miscDefaults.SALAMANDER_ROOT_DIRECTORY + "/build/sceneFragment.spv";

    Shader::createShader(vertexBytecodeFilePath, VK_SHADER_STAGE_VERTEX_BIT, *m_vulkanLogicalDevice, m_scenePipelineComponents.pipelineShaders.vertexShader);
    Shader::createShader(fragmentBytecodeFilePath, VK_SHADER_STAGE_FRAGMENT_BIT, *m_vulkanLogicalDevice, m_scenePipelineComponents.pipelineShaders.fragmentShader);
    
    VkPipelineShaderStageCreateInfo shaderStageCreateInfos[] = {m_scenePipelineComponents.pipelineShaders.vertexShader.shaderStageCreateInfo, m_scenePipelineComponents.pipelineShaders.fragmentShader.shaderStageCreateInfo};
    

    ResourceDescriptor::populateBindingDescription(sizeof(ModelHandler::Vertex), ModelHandler::preservedSceneBindingDescription);
    ResourceDescriptor::fetchSceneAttributeDescriptions(ModelHandler::preservedSceneAttributeDescriptions);
    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
    ModelHandler::populateVertexInputCreateInfo(ModelHandler::preservedSceneAttributeDescriptions, &ModelHandler::preservedSceneBindingDescription, vertexInputCreateInfo);


    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
    ModelHandler::populateInputAssemblyCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE, inputAssemblyCreateInfo);


    VkPipelineViewportStateCreateInfo viewportCreateInfo{};
    populateViewportCreateInfo(1, 1, viewportCreateInfo);

    
    VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo{};
    populateRasterizationCreateInfo(VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, rasterizationCreateInfo);


    VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo{};
    populateMultisamplingCreateInfo(msaaSampleCount, 0.2f, multisamplingCreateInfo);


    VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo{};
    populateDepthStencilCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL, depthStencilCreateInfo);


    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo{};
    populateColorBlendComponents(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, VK_FALSE, colorBlendAttachment, colorBlendCreateInfo);

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicStatesCreateInfo{};
    populateDynamicStatesCreateInfo(dynamicStates, dynamicStatesCreateInfo);


    createMemberPipelineLayout(m_scenePipelineComponents.descriptorSetLayout, m_scenePipelineComponents.pipelineLayout);


    VkGraphicsPipelineCreateInfo scenePipelineCreateInfo{};
    scenePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    
    scenePipelineCreateInfo.stageCount = 2;
    scenePipelineCreateInfo.pStages = shaderStageCreateInfos;
    
    scenePipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
    scenePipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
    scenePipelineCreateInfo.pViewportState = &viewportCreateInfo;
    scenePipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
    scenePipelineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
    scenePipelineCreateInfo.pDepthStencilState = &depthStencilCreateInfo;
    scenePipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
    scenePipelineCreateInfo.pDynamicState = &dynamicStatesCreateInfo;

    scenePipelineCreateInfo.layout = m_scenePipelineComponents.pipelineLayout;
    scenePipelineCreateInfo.renderPass = m_scenePipelineComponents.renderPass;
    scenePipelineCreateInfo.subpass = 0;

    scenePipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    scenePipelineCreateInfo.basePipelineIndex = -1;

    size_t scenePipelineCreationResult = vkCreateGraphicsPipelines(*m_vulkanLogicalDevice, VK_NULL_HANDLE, 1, &scenePipelineCreateInfo, nullptr, &m_scenePipelineComponents.pipeline);
    if (scenePipelineCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create scene graphics pipeline.");
    }


    vkDestroyShaderModule(*m_vulkanLogicalDevice, m_scenePipelineComponents.pipelineShaders.fragmentShader.shaderModule, nullptr);
    vkDestroyShaderModule(*m_vulkanLogicalDevice, m_scenePipelineComponents.pipelineShaders.vertexShader.shaderModule, nullptr);
}

void Renderer::createMemberCubemapPipeline()
{
    std::string vertexBytecodeFilePath = Defaults::miscDefaults.SALAMANDER_ROOT_DIRECTORY + "/build/cubemapVertex.spv";
    std::string fragmentBytecodeFilePath = Defaults::miscDefaults.SALAMANDER_ROOT_DIRECTORY + "/build/cubemapFragment.spv";

    Shader::createShader(vertexBytecodeFilePath, VK_SHADER_STAGE_VERTEX_BIT, *m_vulkanLogicalDevice, m_cubemapPipelineComponents.pipelineShaders.vertexShader);
    Shader::createShader(fragmentBytecodeFilePath, VK_SHADER_STAGE_FRAGMENT_BIT, *m_vulkanLogicalDevice, m_cubemapPipelineComponents.pipelineShaders.fragmentShader);
    
    VkPipelineShaderStageCreateInfo shaderStageCreateInfos[] = {m_cubemapPipelineComponents.pipelineShaders.vertexShader.shaderStageCreateInfo, m_cubemapPipelineComponents.pipelineShaders.fragmentShader.shaderStageCreateInfo};
    
    ResourceDescriptor::populateBindingDescription(sizeof(uint32_t), ModelHandler::preservedCubemapBindingDescription);  // we are only passing the position attribute to the vertex shader.
    ResourceDescriptor::fetchCubemapAttributeDescriptions(ModelHandler::preservedCubemapAttributeDescriptions);
    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
    ModelHandler::populateVertexInputCreateInfo(ModelHandler::preservedCubemapAttributeDescriptions, &ModelHandler::preservedCubemapBindingDescription, vertexInputCreateInfo);


    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
    ModelHandler::populateInputAssemblyCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE, inputAssemblyCreateInfo);


    VkPipelineViewportStateCreateInfo viewportCreateInfo{};
    populateViewportCreateInfo(1, 1, viewportCreateInfo);

    
    VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo{};
    populateRasterizationCreateInfo(VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, rasterizationCreateInfo);


    VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo{};
    populateMultisamplingCreateInfo(VK_SAMPLE_COUNT_1_BIT, 1.0f, multisamplingCreateInfo);


    VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo{};
    populateDepthStencilCreateInfo(VK_FALSE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL, depthStencilCreateInfo);


    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo{};
    populateColorBlendComponents(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, VK_FALSE, colorBlendAttachment, colorBlendCreateInfo);

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicStatesCreateInfo{};
    populateDynamicStatesCreateInfo(dynamicStates, dynamicStatesCreateInfo);


    createMemberPipelineLayout(m_scenePipelineComponents.descriptorSetLayout, m_cubemapPipelineComponents.pipelineLayout);


    VkGraphicsPipelineCreateInfo cubemapPipelineCreateInfo{};
    cubemapPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    
    cubemapPipelineCreateInfo.stageCount = 2;
    cubemapPipelineCreateInfo.pStages = shaderStageCreateInfos;
    
    cubemapPipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
    cubemapPipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
    cubemapPipelineCreateInfo.pViewportState = &viewportCreateInfo;
    cubemapPipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
    cubemapPipelineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
    cubemapPipelineCreateInfo.pDepthStencilState = &depthStencilCreateInfo;
    cubemapPipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
    cubemapPipelineCreateInfo.pDynamicState = &dynamicStatesCreateInfo;

    cubemapPipelineCreateInfo.layout = m_cubemapPipelineComponents.pipelineLayout;
    cubemapPipelineCreateInfo.renderPass = m_cubemapPipelineComponents.renderPass;
    cubemapPipelineCreateInfo.subpass = 0;

    cubemapPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    cubemapPipelineCreateInfo.basePipelineIndex = -1;

    size_t cubemapPipelineCreationResult = vkCreateGraphicsPipelines(*m_vulkanLogicalDevice, VK_NULL_HANDLE, 1, &cubemapPipelineCreateInfo, nullptr, &m_cubemapPipelineComponents.pipeline);
    if (cubemapPipelineCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create cubemap graphics pipeline.");
    }

    vkDestroyShaderModule(*m_vulkanLogicalDevice, m_cubemapPipelineComponents.pipelineShaders.fragmentShader.shaderModule, nullptr);
    vkDestroyShaderModule(*m_vulkanLogicalDevice, m_cubemapPipelineComponents.pipelineShaders.vertexShader.shaderModule, nullptr);
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
        SwapchainHandler::recreateSwapchain(DeviceHandler::VulkanDevices{vulkanPhysicalDevice, *m_vulkanLogicalDevice}, m_scenePipelineComponents.renderPass, m_graphicsCommandPool, displayDetails.vulkanDisplayDetails.graphicsQueue, displayDetails);
        m_mainCamera.swapchainImageExtent = displayDetails.vulkanDisplayDetails.swapchainImageExtent;
        return;
    } else if (imageAcquisitionResult != VK_SUCCESS && imageAcquisitionResult != VK_SUBOPTIMAL_KHR) {
        throwDebugException("Failed to acquire swapchain image.");
    }
    

    vkResetFences(*m_vulkanLogicalDevice, 1, &m_inFlightFences[m_currentFrame]);  // reset fences only after successful image acquisition.


    vkResetCommandBuffer(m_graphicsCommandBuffers[m_currentFrame], 0);  // 0 for no additional flags.
    CommandManager::recordGraphicsCommandBufferCommands(m_graphicsCommandBuffers[m_currentFrame], displayDetails.vulkanDisplayDetails.swapchainFramebuffers[swapchainImageIndex], displayDetails.vulkanDisplayDetails.swapchainImageExtent, m_scenePipelineComponents, m_mainModel.shaderBufferComponents, m_cubemapPipelineComponents, m_cubemapModel.shaderBufferComponents, m_currentFrame);

    
    Uniform::updateFrameUniformBuffers(m_mainCamera, m_mainModel.meshQuaternion, m_currentFrame, displayDetails.glfwWindow, displayDetails.vulkanDisplayDetails.swapchainImageExtent, m_scenePipelineComponents.mappedUniformBuffersMemory, m_cubemapPipelineComponents.mappedUniformBuffersMemory);

    
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
    
    createMemberSceneRenderPass(displayDetails.vulkanDisplayDetails.swapchainImageFormat, displayDetails.vulkanDisplayDetails.msaaSampleCount, vulkanPhysicalDevice);
    createMemberCubemapRenderPass(displayDetails.vulkanDisplayDetails.swapchainImageFormat, vulkanPhysicalDevice);
    
    VkDescriptorSetLayoutBinding sceneUniformBufferLayoutBinding{};
    ResourceDescriptor::populateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, (VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), sceneUniformBufferLayoutBinding);
    
    VkDescriptorSetLayoutBinding sceneCombinedSamplerLayoutBinding{};
    ResourceDescriptor::populateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, sceneCombinedSamplerLayoutBinding);

    std::vector<VkDescriptorSetLayoutBinding> sceneDescriptorSetLayoutBindings = {sceneUniformBufferLayoutBinding, sceneCombinedSamplerLayoutBinding};
    ResourceDescriptor::createDescriptorSetLayout(sceneDescriptorSetLayoutBindings, *m_vulkanLogicalDevice, m_scenePipelineComponents.descriptorSetLayout);
    
    createMemberScenePipeline(displayDetails.vulkanDisplayDetails.msaaSampleCount);

    
    VkDescriptorSetLayoutBinding cubemapUniformBufferLayoutBinding{};
    ResourceDescriptor::populateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, cubemapUniformBufferLayoutBinding);
    
    VkDescriptorSetLayoutBinding cubemapCombinedSamplerLayoutBinding{};
    ResourceDescriptor::populateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, cubemapCombinedSamplerLayoutBinding);

    std::vector<VkDescriptorSetLayoutBinding> cubemapDescriptorSetLayoutBindings = {cubemapUniformBufferLayoutBinding, cubemapCombinedSamplerLayoutBinding};
    ResourceDescriptor::createDescriptorSetLayout(cubemapDescriptorSetLayoutBindings, *m_vulkanLogicalDevice, m_cubemapPipelineComponents.descriptorSetLayout);
    
    createMemberCubemapPipeline();

    
    CommandManager::createGraphicsCommandPool(graphicsFamilyIndex, *m_vulkanLogicalDevice, m_graphicsCommandPool);

    // populate the color image details.
    Image::populateImageDetails(displayDetails.vulkanDisplayDetails.swapchainImageExtent.width, displayDetails.vulkanDisplayDetails.swapchainImageExtent.height, 1, 1, displayDetails.vulkanDisplayDetails.msaaSampleCount, displayDetails.vulkanDisplayDetails.swapchainImageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, temporaryVulkanDevices, displayDetails.vulkanDisplayDetails.colorImageDetails);
    Image::createImageView(displayDetails.vulkanDisplayDetails.colorImageDetails.image, displayDetails.vulkanDisplayDetails.colorImageDetails.imageFormat, 1, 1, VK_IMAGE_ASPECT_COLOR_BIT, *m_vulkanLogicalDevice, displayDetails.vulkanDisplayDetails.colorImageDetails.imageView);
    
    Depth::populateDepthImageDetails(displayDetails.vulkanDisplayDetails.swapchainImageExtent, displayDetails.vulkanDisplayDetails.msaaSampleCount, m_graphicsCommandPool, displayDetails.vulkanDisplayDetails.graphicsQueue, temporaryVulkanDevices, displayDetails.vulkanDisplayDetails.depthImageDetails);

    SwapchainHandler::createSwapchainFramebuffers(displayDetails.vulkanDisplayDetails.swapchainImageViews, displayDetails.vulkanDisplayDetails.colorImageDetails.imageView, displayDetails.vulkanDisplayDetails.depthImageDetails.imageView, m_scenePipelineComponents.renderPass, displayDetails.vulkanDisplayDetails.swapchainImageExtent, *m_vulkanLogicalDevice, displayDetails.vulkanDisplayDetails.swapchainFramebuffers);  // in render function due to timing of swapchain framebuffer creation.

    m_mainModel.loadModelFromAbsolutePath((Defaults::miscDefaults.SALAMANDER_ROOT_DIRECTORY + "/assets/models/Avocado/Avocado.gltf"));
    // m_mainModel.normalizeNormalValues();
    // TODO: add seperate "transfer" queue(see vulkan-tutorial page).
    m_mainModel.populateShaderBufferComponents(m_graphicsCommandPool, displayDetails.vulkanDisplayDetails.graphicsQueue, temporaryVulkanDevices);
    Image::populateTextureDetails(m_mainModel.absoluteTextureImagePath, false, m_graphicsCommandPool, displayDetails.vulkanDisplayDetails.graphicsQueue, temporaryVulkanDevices, m_mainModel.textureDetails);

    m_cubemapModel.loadModelFromAbsolutePath((Defaults::miscDefaults.SALAMANDER_ROOT_DIRECTORY + "/assets/models/CubemapCube.gltf"));
    m_cubemapModel.populateShaderBufferComponents(m_graphicsCommandPool, displayDetails.vulkanDisplayDetails.graphicsQueue, temporaryVulkanDevices);
    Image::populateTextureDetails((Defaults::miscDefaults.SALAMANDER_ROOT_DIRECTORY + "/assets/skyboxes/field"), true, m_graphicsCommandPool, displayDetails.vulkanDisplayDetails.graphicsQueue, temporaryVulkanDevices, m_cubemapTextureDetails);

    Uniform::createUniformBuffers(sizeof(Uniform::SceneUniformBufferObject), temporaryVulkanDevices, m_scenePipelineComponents.uniformBuffers, m_scenePipelineComponents.uniformBuffersMemory, m_scenePipelineComponents.mappedUniformBuffersMemory);
    
    ResourceDescriptor::createDescriptorPool(*m_vulkanLogicalDevice, m_scenePipelineComponents.descriptorPool);
    ResourceDescriptor::createDescriptorSets(m_scenePipelineComponents.descriptorSetLayout, m_scenePipelineComponents.descriptorPool, *m_vulkanLogicalDevice, m_scenePipelineComponents.descriptorSets);
    ResourceDescriptor::populateDescriptorSets(m_scenePipelineComponents.uniformBuffers, m_mainModel.textureDetails.textureImageDetails.imageView, m_mainModel.textureDetails.textureSampler, *m_vulkanLogicalDevice, m_scenePipelineComponents.descriptorSets);

    Uniform::createUniformBuffers(sizeof(Uniform::CubemapUniformBufferObject), temporaryVulkanDevices, m_cubemapPipelineComponents.uniformBuffers, m_cubemapPipelineComponents.uniformBuffersMemory, m_cubemapPipelineComponents.mappedUniformBuffersMemory);

    // cubemap shaders conviently use the same uniforms/only uniform difference is shader stages.
    ResourceDescriptor::createDescriptorPool(*m_vulkanLogicalDevice, m_cubemapPipelineComponents.descriptorPool);
    ResourceDescriptor::createDescriptorSets(m_cubemapPipelineComponents.descriptorSetLayout, m_cubemapPipelineComponents.descriptorPool, *m_vulkanLogicalDevice, m_cubemapPipelineComponents.descriptorSets);
    ResourceDescriptor::populateDescriptorSets(m_cubemapPipelineComponents.uniformBuffers, m_cubemapTextureDetails.textureImageDetails.imageView, m_cubemapTextureDetails.textureSampler, *m_vulkanLogicalDevice, m_cubemapPipelineComponents.descriptorSets);

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
    }

    vkDestroyCommandPool(*m_vulkanLogicalDevice, m_graphicsCommandPool, nullptr);  // child command buffers automatically freed.

    m_scenePipelineComponents.cleanupPipelineComponents(*m_vulkanLogicalDevice);
    m_cubemapPipelineComponents.cleanupPipelineComponents(*m_vulkanLogicalDevice);    
}

Renderer::Renderer()
{
    
}
