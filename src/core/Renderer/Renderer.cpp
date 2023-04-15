#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Renderer/Renderer.h>
#include <core/Shader/Shader.h>
#include <core/DisplayManager/DisplayManager.h>
#include <core/VulkanInstance/VulkanInstance.h>
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

void Renderer::createRenderPass(VkFormat swapchainImageFormat, VkRenderPass& renderPass)
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
    
    size_t renderPassCreationResult = vkCreateRenderPass(*m_vulkanLogicalDevice, &renderPassCreateInfo, nullptr, &renderPass);
    if (renderPassCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create render pass.");
    }
}

void Renderer::fillVertexInputCreateInfo(VkPipelineVertexInputStateCreateInfo& vertexInputCreateInfo)
{
    vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    
    vertexInputCreateInfo.vertexBindingDescriptionCount = 0;  // vertex binding details struct count.
    vertexInputCreateInfo.pVertexBindingDescriptions = nullptr;
    vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;  // vertex attribute details struct count.
    vertexInputCreateInfo.pVertexAttributeDescriptions = nullptr;
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
    rasterizationCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;  // vertex order for faces to be considered front-facing.
    
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

void Renderer::createPipelineLayout(VkPipelineLayout& pipelineLayout)
{
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.flags = 0;
    
    pipelineLayoutCreateInfo.setLayoutCount = 0;
    pipelineLayoutCreateInfo.pSetLayouts = nullptr;
    
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

    size_t pipelineLayoutCreationResult = vkCreatePipelineLayout(*m_vulkanLogicalDevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
    if (pipelineLayoutCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create pipeline layout.");
    }
}

void Renderer::createGraphicsPipeline(VkRenderPass renderPass, VkPipeline& graphicsPipeline)
{
    const char *vertexBytecodeFilePath = "/home/lucas/programming/graphics/salamander-engine/build/vertex.spv";
    const char *fragmentBytecodeFilePath = "/home/lucas/programming/graphics/salamander-engine/build/fragment.spv";

    Shader::PipelineShaders pipelineShaders;
    Shader::createShader(vertexBytecodeFilePath, VK_SHADER_STAGE_VERTEX_BIT, *m_vulkanLogicalDevice, pipelineShaders.vertexShader);
    Shader::createShader(fragmentBytecodeFilePath, VK_SHADER_STAGE_FRAGMENT_BIT, *m_vulkanLogicalDevice, pipelineShaders.fragmentShader);
    
    VkPipelineShaderStageCreateInfo shaderStageCreateInfos[] = {pipelineShaders.vertexShader.shaderStageCreateInfo, pipelineShaders.fragmentShader.shaderStageCreateInfo};
    

    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
    fillVertexInputCreateInfo(vertexInputCreateInfo);


    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
    fillInputAssemblyCreateInfo(inputAssemblyCreateInfo);


    /*
    VkViewport defaultViewport{};
    defaultViewport.x = 0.0f;
    defaultViewport.y = 0.0f;
    defaultViewport.width = (float)(m_instance.m_displayDetails.swapchainExtent.width);
    defaultViewport.height = (float)(m_instance.m_displayDetails.swapchainExtent.height);
    defaultViewport.minDepth = 0.0f;
    defaultViewport.maxDepth = 1.0f;


    VkRect2D defaultScissor{};
    defaultScissor.offset = {0, 0};
    defaultScissor.extent = m_instance.m_diplayDetails.swapchainExtent;
    */


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


    VkPipelineLayout pipelineLayout;
    createPipelineLayout(pipelineLayout);


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

    pipelineCreateInfo.layout = pipelineLayout;
    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.subpass = 0;

    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = -1;

    size_t pipelineCreationResult = vkCreateGraphicsPipelines(*m_vulkanLogicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &graphicsPipeline);
    if (pipelineCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create graphics pipeline.");
    }


    vkDestroyShaderModule(*m_vulkanLogicalDevice, pipelineShaders.fragmentShader.shaderModule, nullptr);
    vkDestroyShaderModule(*m_vulkanLogicalDevice, pipelineShaders.vertexShader.shaderModule, nullptr);


    // inefficient, only to make side-effects more visible.
    m_renderPass = renderPass;
    m_pipelineShaders = pipelineShaders;
    m_pipelineLayout = pipelineLayout;
}

void Renderer::setVulkanLogicalDevice(VkDevice *vulkanLogicalDevice)
{
    m_vulkanLogicalDevice = vulkanLogicalDevice;
}

void Renderer::render(DisplayManager::DisplayDetails displayDetails)
{
    VkRenderPass renderPass;
    createRenderPass(displayDetails.vulkanDisplayDetails.swapchainImageFormat, renderPass);
    
    createGraphicsPipeline(renderPass, m_graphicsPipeline);

    
    DisplayManager::stallWindow(displayDetails.glfwWindow);
}

void Renderer::cleanupRenderer()
{
    vkDestroyPipeline(*m_vulkanLogicalDevice, m_graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(*m_vulkanLogicalDevice, m_pipelineLayout, nullptr);
    vkDestroyRenderPass(*m_vulkanLogicalDevice, m_renderPass, nullptr);
}

Renderer::Renderer()
{
    
}
