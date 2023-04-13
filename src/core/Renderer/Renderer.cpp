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


Renderer::Renderer()
{
    
}

void Renderer::render(VkDevice vulkanLogicalDevice, DisplayManager::DisplayDetails displayDetails)
{
    this->createGraphicsPipeline(vulkanLogicalDevice);
    
    DisplayManager::stallWindow(displayDetails.glfwWindow);
}

void Renderer::createGraphicsPipeline(VkDevice vulkanLogicalDevice)
{
    m_shaderStages.vertexShader.shaderFilePath = "/home/lucas/programming/graphics/salamander-engine/include/shaders/triangle.vert";
    Shader::completeShaderData(VK_SHADER_STAGE_VERTEX_BIT, vulkanLogicalDevice, m_shaderStages.vertexShader);

    m_shaderStages.fragmentShader.shaderFilePath = "/home/lucas/programming/graphics/salamander-engine/include/shaders/triangle.frag";
    Shader::completeShaderData(VK_SHADER_STAGE_FRAGMENT_BIT, vulkanLogicalDevice, m_shaderStages.fragmentShader);

    // VkPipelineShaderStageCreateInfo shaderStages[] = {m_shaderStages.vertexShader.shaderStageCreateInfo, m_shaderStages.fragmentShader.shaderStageCreateInfo};
    

    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
    vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputCreateInfo.vertexBindingDescriptionCount = 0;  // vertex binding details struct count.
    vertexInputCreateInfo.pVertexBindingDescriptions = nullptr;
    vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;  // vertex attribute details struct count.
    vertexInputCreateInfo.pVertexAttributeDescriptions = nullptr;


    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
    inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;  // form triangle primitives without vertex reuse.
    inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;  // disable the possibility of vertex reuse.


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


    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(m_dynamicStates.size());
    dynamicStateCreateInfo.pDynamicStates = m_dynamicStates.data();


    VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.scissorCount = 1;


    VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo{};
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


    // currently disabled.
    VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo{};
    multisamplingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisamplingCreateInfo.sampleShadingEnable = VK_FALSE;
    multisamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisamplingCreateInfo.minSampleShading = 1.0f;
    multisamplingCreateInfo.pSampleMask = nullptr;
    multisamplingCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multisamplingCreateInfo.alphaToOneEnable = VK_FALSE;


    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;


    VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo{};
    colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
    colorBlendCreateInfo.attachmentCount = 1;
    colorBlendCreateInfo.pAttachments = &colorBlendAttachment;
    colorBlendCreateInfo.blendConstants[0] = 0.0f;
    colorBlendCreateInfo.blendConstants[1] = 0.0f;
    colorBlendCreateInfo.blendConstants[2] = 0.0f;
    colorBlendCreateInfo.blendConstants[3] = 0.0f;


    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 0;
    pipelineLayoutCreateInfo.pSetLayouts = nullptr;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.flags = 0;

    size_t pipelineLayoutCreationResult = vkCreatePipelineLayout(vulkanLogicalDevice, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout);
    if (pipelineLayoutCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create pipeline layout.");
    }


    vkDestroyShaderModule(vulkanLogicalDevice, m_shaderStages.fragmentShader.shaderModule, nullptr);
    vkDestroyShaderModule(vulkanLogicalDevice, m_shaderStages.vertexShader.shaderModule, nullptr);
}

void Renderer::cleanupRenderer(VkDevice vulkanLogicalDevice)
{
    vkDestroyPipelineLayout(vulkanLogicalDevice, m_pipelineLayout, nullptr);
}
