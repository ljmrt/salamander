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
#include <core/VulkanInstance/DeviceHandler.h>
#include <core/Model/ModelHandler.h>
#include <core/Logging/ErrorLogger.h>
#include <core/Defaults/Defaults.h>

#include <cstdint>
#include <string>
#include <vector>


void RendererDetails::populateColorAttachmentComponents(VkFormat swapchainImageFormat, VkSampleCountFlagBits msaaSampleCount, VkAttachmentDescription& colorAttachmentDescription, VkAttachmentReference& colorAttachmentReference, VkAttachmentDescription& colorAttachmentResolveDescription, VkAttachmentReference& colorAttachmentResolveReference)
{
    colorAttachmentDescription.format = swapchainImageFormat;
    colorAttachmentDescription.samples = msaaSampleCount;
    
    colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
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

void RendererDetails::populateDepthAttachmentComponents(VkSampleCountFlagBits msaaSampleCount, VkAttachmentStoreOp storeOp, uint32_t attachment, VkPhysicalDevice vulkanPhysicalDevice, VkAttachmentDescription& depthAttachmentDescription, VkAttachmentReference& depthAttachmentReference)
{
    VkFormat depthAttachmentDescriptionFormat;
    Depth::selectDepthImageFormat(vulkanPhysicalDevice, depthAttachmentDescriptionFormat);
    depthAttachmentDescription.format = depthAttachmentDescriptionFormat;
    depthAttachmentDescription.samples = msaaSampleCount;

    depthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachmentDescription.storeOp = storeOp;
    depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;  // we have no need for the previous depth buffer's contents.
    if (storeOp == VK_ATTACHMENT_STORE_OP_STORE) {  // the attachment will be read later.
        depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;  // allows use as a depth attachment(writing) and shader-accessible image(reading, through a combined sampler).
    } else {
        depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }

    depthAttachmentReference.attachment = attachment;
    depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
}

void RendererDetails::Renderer::createMemberRenderPass(VkFormat swapchainImageFormat, VkSampleCountFlagBits msaaSampleCount, VkPhysicalDevice vulkanPhysicalDevice)
{
    VkAttachmentDescription colorAttachmentDescription{};
    VkAttachmentReference colorAttachmentReference{};
    // color attachment resolve "resolves" the general color attachment for presentation.
    VkAttachmentDescription colorAttachmentResolveDescription{};
    VkAttachmentReference colorAttachmentResolveReference{};
    populateColorAttachmentComponents(swapchainImageFormat, msaaSampleCount, colorAttachmentDescription, colorAttachmentReference, colorAttachmentResolveDescription, colorAttachmentResolveReference);


    VkAttachmentDescription depthAttachmentDescription{};
    VkAttachmentReference depthAttachmentReference{};
    populateDepthAttachmentComponents(msaaSampleCount, VK_ATTACHMENT_STORE_OP_DONT_CARE, 1, vulkanPhysicalDevice, depthAttachmentDescription, depthAttachmentReference);
    
    
    VkSubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorAttachmentReference;
    subpassDescription.pResolveAttachments = &colorAttachmentResolveReference;
    subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;

    
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
        throwDebugException("Failed to create member render pass.");
    }
}

void RendererDetails::createShadowRenderPass(DeviceHandler::VulkanDevices vulkanDevices, VkRenderPass& renderPass)
{
    VkAttachmentDescription depthAttachmentDescription{};
    VkAttachmentReference depthAttachmentReference{};
    RendererDetails::populateDepthAttachmentComponents(VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_STORE_OP_STORE, 0, vulkanDevices.physicalDevice, depthAttachmentDescription, depthAttachmentReference);
    
    
    VkSubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    
    subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;

    
    VkSubpassDependency depthAttachmentSubpassDependency{};
    
    depthAttachmentSubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    depthAttachmentSubpassDependency.dstSubpass = 0;

    // operations to wait on/operations that should wait.
    depthAttachmentSubpassDependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    depthAttachmentSubpassDependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    
    depthAttachmentSubpassDependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    depthAttachmentSubpassDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    // ensure depth stencil attachment is wrote before it is read.
    VkSubpassDependency shaderReadSubpassDependency{};

    shaderReadSubpassDependency.srcSubpass = 0;
    shaderReadSubpassDependency.dstSubpass = VK_SUBPASS_EXTERNAL;

    shaderReadSubpassDependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    shaderReadSubpassDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    
    shaderReadSubpassDependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    shaderReadSubpassDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;


    std::array<VkAttachmentDescription, 1> attachmentDescriptions = {depthAttachmentDescription};
    std::array<VkSubpassDependency, 2> subpassDependencies = {depthAttachmentSubpassDependency, shaderReadSubpassDependency};
    
    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    
    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
    renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
    
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;

    renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
    renderPassCreateInfo.pDependencies = subpassDependencies.data();

    
    size_t renderPassCreationResult = vkCreateRenderPass(vulkanDevices.logicalDevice, &renderPassCreateInfo, nullptr, &renderPass);
    if (renderPassCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create shadow render pass.");
    }
}

void RendererDetails::populateViewportCreateInfo(uint32_t viewportCount, uint32_t scissorCount, VkPipelineViewportStateCreateInfo& viewportCreateInfo)
{
    viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

    // dynamic viewport and scissor setting.
    viewportCreateInfo.viewportCount = viewportCount;
    viewportCreateInfo.scissorCount = scissorCount;
}

void RendererDetails::populateRasterizationCreateInfo(VkCullModeFlags cullMode, VkFrontFace frontFace, VkPipelineRasterizationStateCreateInfo& rasterizationCreateInfo)
{
    rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    
    rasterizationCreateInfo.depthClampEnable = VK_FALSE;  // default to discarding rather than clamping fragments beyond the near and far planes. 
    rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;  // pass through the rasterization stage.
    
    // wireframe mode(fragments for only the edges of polygons): polygon mode VK_POLYGON_MODE_LINE
    // point mode(fragments for only the polygon vertices): polygon mode VK_POLYGON_MODE_POINT
    rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;  // fill the area of the polygon with fragments.
    
    rasterizationCreateInfo.lineWidth = 1.0f;
    
    rasterizationCreateInfo.cullMode = cullMode;
    rasterizationCreateInfo.frontFace = frontFace;
    
    rasterizationCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationCreateInfo.depthBiasConstantFactor = 0.0f;
    rasterizationCreateInfo.depthBiasClamp = 0.0f;
    rasterizationCreateInfo.depthBiasSlopeFactor = 0.0f;
}

void RendererDetails::fetchMaximumUsableSampleCount(VkPhysicalDevice vulkanPhysicalDevice, VkSampleCountFlagBits& maximumUsableSampleCount)
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

void RendererDetails::populateMultisamplingCreateInfo(VkSampleCountFlagBits rasterizationSamples, float minSampleShading, VkPipelineMultisampleStateCreateInfo& multisamplingCreateInfo)
{
    multisamplingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    
    multisamplingCreateInfo.sampleShadingEnable = VK_TRUE;
    multisamplingCreateInfo.rasterizationSamples = rasterizationSamples;
    multisamplingCreateInfo.minSampleShading = minSampleShading;
    multisamplingCreateInfo.pSampleMask = nullptr;
    
    multisamplingCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multisamplingCreateInfo.alphaToOneEnable = VK_FALSE;
}

void RendererDetails::populateDepthStencilCreateInfo(VkBool32 depthTestEnable, VkBool32 depthWriteEnable, VkCompareOp depthCompareOp, VkPipelineDepthStencilStateCreateInfo& depthStencilCreateInfo)
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

void RendererDetails::populateColorBlendComponents(VkColorComponentFlags colorWriteMask, VkBool32 blendEnable, VkPipelineColorBlendAttachmentState& colorBlendAttachment, VkPipelineColorBlendStateCreateInfo& colorBlendCreateInfo)
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

void RendererDetails::populateDynamicStatesCreateInfo(std::vector<VkDynamicState>& dynamicStates, VkPipelineDynamicStateCreateInfo& dynamicStatesCreateInfo)
{
    dynamicStatesCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    
    dynamicStatesCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicStatesCreateInfo.pDynamicStates = dynamicStates.data();    
}

void RendererDetails::createPipelineLayout(VkDevice vulkanLogicalDevice, VkDescriptorSetLayout& descriptorSetLayout, std::optional<VkPushConstantRange *> pushConstant, VkPipelineLayout& pipelineLayout)
{
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.flags = 0;
    
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
    
    pipelineLayoutCreateInfo.pushConstantRangeCount = (pushConstant.has_value() ? 1 : 0);
    pipelineLayoutCreateInfo.pPushConstantRanges = pushConstant.value_or(nullptr);

    VkResult pipelineLayoutCreationResult = vkCreatePipelineLayout(vulkanLogicalDevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
    if (pipelineLayoutCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create member pipeline layout.");
    }
}

void RendererDetails::Renderer::createMemberCubemapPipeline(VkSampleCountFlagBits msaaSampleCount)
{
    Pipeline::PipelineData cubemapPipelineData;

    cubemapPipelineData.vulkanLogicalDevice = *m_vulkanLogicalDevice;
    
    cubemapPipelineData.vertexShaderBytecodeAbsolutePath = (Defaults::applicationDefaults.SALAMANDER_ROOT_DIRECTORY + "/build/cubemapVertex.spv");
    cubemapPipelineData.geometryShaderBytecodeAbsolutePath = "*NA*";
    cubemapPipelineData.fragmentShaderBytecodeAbsolutePath = (Defaults::applicationDefaults.SALAMANDER_ROOT_DIRECTORY + "/build/cubemapFragment.spv");

    cubemapPipelineData.vertexDataStride = sizeof(ModelHandler::CubemapVertexData);
    cubemapPipelineData.fetchAttributeDescriptions = ResourceDescriptor::fetchCubemapAttributeDescriptions;

    cubemapPipelineData.inputAssemblyTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    cubemapPipelineData.inputAssemblyPrimitiveRestartEnable = VK_FALSE;

    cubemapPipelineData.viewportViewportCount = 1;
    cubemapPipelineData.viewportScissorCount = 1;
    
    cubemapPipelineData.rasterizationCullMode = VK_CULL_MODE_FRONT_BIT;
    cubemapPipelineData.rasterizationFrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

    cubemapPipelineData.multisamplingRasterizationSamples = msaaSampleCount;
    cubemapPipelineData.multisamplingMinSampleShading = 0.2f;

    cubemapPipelineData.depthStencilDepthTestEnable = VK_TRUE;
    cubemapPipelineData.depthStencilDepthWriteEnable = VK_TRUE;
    cubemapPipelineData.depthStencilDepthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

    cubemapPipelineData.colorBlendColorWriteMask = (VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
    cubemapPipelineData.colorBlendBlendEnable = VK_TRUE;

    cubemapPipelineData.dynamicStatesDynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    cubemapPipelineData.pipelineRenderPass = m_renderPass;


    m_cubemapPipelineComponents.createMemberPipeline(cubemapPipelineData);
}

void RendererDetails::Renderer::createMemberScenePipeline(VkSampleCountFlagBits msaaSampleCount)
{
    Pipeline::PipelineData scenePipelineData;

    scenePipelineData.vulkanLogicalDevice = *m_vulkanLogicalDevice;
    
    scenePipelineData.vertexShaderBytecodeAbsolutePath = (Defaults::applicationDefaults.SALAMANDER_ROOT_DIRECTORY + "/build/sceneVertex.spv");
    scenePipelineData.geometryShaderBytecodeAbsolutePath = "*NA*";
    scenePipelineData.fragmentShaderBytecodeAbsolutePath = (Defaults::applicationDefaults.SALAMANDER_ROOT_DIRECTORY + "/build/sceneFragment.spv");

    scenePipelineData.vertexDataStride = sizeof(ModelHandler::SceneVertexData);
    scenePipelineData.fetchAttributeDescriptions = ResourceDescriptor::fetchSceneAttributeDescriptions;

    scenePipelineData.inputAssemblyTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    scenePipelineData.inputAssemblyPrimitiveRestartEnable = VK_FALSE;

    scenePipelineData.viewportViewportCount = 1;
    scenePipelineData.viewportScissorCount = 1;
    
    scenePipelineData.rasterizationCullMode = VK_CULL_MODE_BACK_BIT;
    scenePipelineData.rasterizationFrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

    scenePipelineData.multisamplingRasterizationSamples = msaaSampleCount;
    scenePipelineData.multisamplingMinSampleShading = 0.2f;

    scenePipelineData.depthStencilDepthTestEnable = VK_TRUE;
    scenePipelineData.depthStencilDepthWriteEnable = VK_TRUE;
    scenePipelineData.depthStencilDepthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

    scenePipelineData.colorBlendColorWriteMask = (VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
    scenePipelineData.colorBlendBlendEnable = VK_TRUE;

    scenePipelineData.dynamicStatesDynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    scenePipelineData.pipelineRenderPass = m_renderPass;


    m_scenePipelineComponents.createMemberPipeline(scenePipelineData);
}

void RendererDetails::Renderer::createMemberSceneNormalsPipeline(VkSampleCountFlagBits msaaSampleCount)
{
    Pipeline::PipelineData sceneNormalsPipelineData;

    sceneNormalsPipelineData.vulkanLogicalDevice = *m_vulkanLogicalDevice;
    
    sceneNormalsPipelineData.vertexShaderBytecodeAbsolutePath = (Defaults::applicationDefaults.SALAMANDER_ROOT_DIRECTORY + "/build/sceneNormalsVertex.spv");
    sceneNormalsPipelineData.geometryShaderBytecodeAbsolutePath = (Defaults::applicationDefaults.SALAMANDER_ROOT_DIRECTORY + "/build/sceneNormalsGeometry.spv");
    sceneNormalsPipelineData.fragmentShaderBytecodeAbsolutePath = (Defaults::applicationDefaults.SALAMANDER_ROOT_DIRECTORY + "/build/sceneNormalsFragment.spv");

    sceneNormalsPipelineData.vertexDataStride = sizeof(ModelHandler::SceneNormalsVertexData);
    sceneNormalsPipelineData.fetchAttributeDescriptions = ResourceDescriptor::fetchSceneNormalsAttributeDescriptions;

    sceneNormalsPipelineData.inputAssemblyTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    sceneNormalsPipelineData.inputAssemblyPrimitiveRestartEnable = VK_FALSE;

    sceneNormalsPipelineData.viewportViewportCount = 1;
    sceneNormalsPipelineData.viewportScissorCount = 1;
    
    sceneNormalsPipelineData.rasterizationCullMode = VK_CULL_MODE_BACK_BIT;
    sceneNormalsPipelineData.rasterizationFrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

    sceneNormalsPipelineData.multisamplingRasterizationSamples = msaaSampleCount;
    sceneNormalsPipelineData.multisamplingMinSampleShading = 0.2f;

    sceneNormalsPipelineData.depthStencilDepthTestEnable = VK_TRUE;
    sceneNormalsPipelineData.depthStencilDepthWriteEnable = VK_TRUE;
    sceneNormalsPipelineData.depthStencilDepthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

    sceneNormalsPipelineData.colorBlendColorWriteMask = (VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
    sceneNormalsPipelineData.colorBlendBlendEnable = VK_TRUE;

    sceneNormalsPipelineData.dynamicStatesDynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    sceneNormalsPipelineData.pipelineRenderPass = m_renderPass;


    m_sceneNormalsPipelineComponents.createMemberPipeline(sceneNormalsPipelineData);
}

void RendererDetails::createDirectionalShadowPipeline(VkRenderPass renderPass, VkDevice vulkanLogicalDevice, Pipeline::PipelineComponents& pipelineComponents)
{
    Pipeline::PipelineData directionalShadowPipelineData;

    directionalShadowPipelineData.vulkanLogicalDevice = vulkanLogicalDevice;
    
    directionalShadowPipelineData.vertexShaderBytecodeAbsolutePath = (Defaults::applicationDefaults.SALAMANDER_ROOT_DIRECTORY + "/build/directionalShadowVertex.spv");
    directionalShadowPipelineData.geometryShaderBytecodeAbsolutePath = "*NA*";
    directionalShadowPipelineData.fragmentShaderBytecodeAbsolutePath = (Defaults::applicationDefaults.SALAMANDER_ROOT_DIRECTORY + "/build/directionalShadowFragment.spv");

    // uses the same vertex data stride and similar as the cubemap pipeline.
    directionalShadowPipelineData.vertexDataStride = sizeof(ModelHandler::ShadowVertexData);
    directionalShadowPipelineData.fetchAttributeDescriptions = ResourceDescriptor::fetchShadowAttributeDescriptions;

    directionalShadowPipelineData.inputAssemblyTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    directionalShadowPipelineData.inputAssemblyPrimitiveRestartEnable = VK_FALSE;

    directionalShadowPipelineData.viewportViewportCount = 1;
    directionalShadowPipelineData.viewportScissorCount = 1;
    
    directionalShadowPipelineData.rasterizationCullMode = VK_CULL_MODE_NONE;
    directionalShadowPipelineData.rasterizationFrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

    directionalShadowPipelineData.multisamplingRasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    directionalShadowPipelineData.multisamplingMinSampleShading = 1.0f;

    directionalShadowPipelineData.depthStencilDepthTestEnable = VK_TRUE;
    directionalShadowPipelineData.depthStencilDepthWriteEnable = VK_TRUE;
    directionalShadowPipelineData.depthStencilDepthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

    directionalShadowPipelineData.colorBlendColorWriteMask = (VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
    directionalShadowPipelineData.colorBlendBlendEnable = VK_TRUE;

    directionalShadowPipelineData.dynamicStatesDynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    directionalShadowPipelineData.pipelineRenderPass = renderPass;

    
    pipelineComponents.createMemberPipeline(directionalShadowPipelineData);
}

void RendererDetails::createPointShadowPipeline(VkRenderPass renderPass, VkDevice vulkanLogicalDevice, Pipeline::PipelineComponents& pipelineComponents)
{
    Pipeline::PipelineData pointShadowPipelineData;

    pointShadowPipelineData.vulkanLogicalDevice = vulkanLogicalDevice;
    
    pointShadowPipelineData.vertexShaderBytecodeAbsolutePath = (Defaults::applicationDefaults.SALAMANDER_ROOT_DIRECTORY + "/build/pointShadowVertex.spv");
    pointShadowPipelineData.geometryShaderBytecodeAbsolutePath = "*NA*";
    pointShadowPipelineData.fragmentShaderBytecodeAbsolutePath = (Defaults::applicationDefaults.SALAMANDER_ROOT_DIRECTORY + "/build/pointShadowFragment.spv");

    // uses the same vertex data stride and similar as the cubemap pipeline.
    pointShadowPipelineData.vertexDataStride = sizeof(ModelHandler::ShadowVertexData);
    pointShadowPipelineData.fetchAttributeDescriptions = ResourceDescriptor::fetchShadowAttributeDescriptions;

    pointShadowPipelineData.inputAssemblyTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pointShadowPipelineData.inputAssemblyPrimitiveRestartEnable = VK_FALSE;

    pointShadowPipelineData.viewportViewportCount = 1;
    pointShadowPipelineData.viewportScissorCount = 1;
    
    pointShadowPipelineData.rasterizationCullMode = VK_CULL_MODE_NONE;
    pointShadowPipelineData.rasterizationFrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

    pointShadowPipelineData.multisamplingRasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    pointShadowPipelineData.multisamplingMinSampleShading = 1.0f;

    pointShadowPipelineData.depthStencilDepthTestEnable = VK_TRUE;
    pointShadowPipelineData.depthStencilDepthWriteEnable = VK_TRUE;
    pointShadowPipelineData.depthStencilDepthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

    pointShadowPipelineData.colorBlendColorWriteMask = (VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
    pointShadowPipelineData.colorBlendBlendEnable = VK_TRUE;

    pointShadowPipelineData.dynamicStatesDynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    pointShadowPipelineData.pipelineRenderPass = renderPass;
    VkPushConstantRange pushConstant{};
    Uniform::populatePushConstant(0, sizeof(Uniform::PointShadowPushConstants), VK_SHADER_STAGE_VERTEX_BIT, pushConstant);
    pointShadowPipelineData.pushConstant = &pushConstant;

    
    pipelineComponents.createMemberPipeline(pointShadowPipelineData);
}

void RendererDetails::Renderer::createMemberSynchronizationObjects()
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

void RendererDetails::Renderer::drawFrame(DisplayManager::DisplayDetails& displayDetails, VkPhysicalDevice vulkanPhysicalDevice, VkQueue graphicsQueue, VkQueue presentationQueue)
{
    vkWaitForFences(*m_vulkanLogicalDevice, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);  // wait for the GPU to finish with the previous frame, UINT64_MAX timeout.


    uint32_t swapchainImageIndex;  // prefer to use size_t, but want to avoid weird casts to uint32_t.
    VkResult imageAcquisitionResult = vkAcquireNextImageKHR(*m_vulkanLogicalDevice, displayDetails.swapchain, UINT64_MAX, m_imageAvailibleSemaphores[m_currentFrame], VK_NULL_HANDLE, &swapchainImageIndex);  // get the index of an availbile swapchain image.

    if (imageAcquisitionResult == VK_ERROR_OUT_OF_DATE_KHR) {
        SwapchainHandler::recreateSwapchain(DeviceHandler::VulkanDevices{vulkanPhysicalDevice, *m_vulkanLogicalDevice}, m_renderPass, m_directionalShadowOperation, m_pointShadowOperation, displayDetails);
        m_mainCamera.swapchainImageExtent = displayDetails.swapchainImageExtent;
        return;
    } else if (imageAcquisitionResult != VK_SUCCESS && imageAcquisitionResult != VK_SUBOPTIMAL_KHR) {
        throwDebugException("Failed to acquire swapchain image.");
    }
    

    vkResetFences(*m_vulkanLogicalDevice, 1, &m_inFlightFences[m_currentFrame]);  // reset fences only after successful image acquisition.


    vkResetCommandBuffer(displayDetails.graphicsCommandBuffers[m_currentFrame], 0);  // 0 for no additional flags.

    CommandManager::GraphicsRecordingPackage graphicsRecordingPackage{};
    graphicsRecordingPackage.graphicsCommandBuffer = displayDetails.graphicsCommandBuffers[m_currentFrame];
    graphicsRecordingPackage.renderPass = m_renderPass;
    
    graphicsRecordingPackage.swapchainImageExtent = displayDetails.swapchainImageExtent;
    graphicsRecordingPackage.swapchainIndexFramebuffer = displayDetails.swapchainFramebuffers[swapchainImageIndex];
    graphicsRecordingPackage.currentFrame = m_currentFrame;
    
    graphicsRecordingPackage.cubemapPipelineComponents = m_cubemapPipelineComponents;
    graphicsRecordingPackage.cubemapShaderBufferComponents = m_cubemapModel.shaderBufferComponents;
    graphicsRecordingPackage.scenePipelineComponents = m_scenePipelineComponents;
    graphicsRecordingPackage.sceneShaderBufferComponents = m_mainModel.shaderBufferComponents;
    graphicsRecordingPackage.sceneNormalsPipelineComponents = m_sceneNormalsPipelineComponents;
    graphicsRecordingPackage.sceneNormalsShaderBufferComponents = m_dummySceneNormalsModel.shaderBufferComponents;
    graphicsRecordingPackage.directionalShadowOperation = m_directionalShadowOperation;
    graphicsRecordingPackage.directionalShadowShaderBufferComponents = m_dummyDirectionalShadowModel.shaderBufferComponents;
    graphicsRecordingPackage.pointShadowOperation = m_pointShadowOperation;
    graphicsRecordingPackage.pointShadowShaderBufferComponents = m_dummyPointShadowModel.shaderBufferComponents;
    
    CommandManager::recordGraphicsCommandBufferCommands(graphicsRecordingPackage);


    Uniform::UniformBuffersUpdatePackage uniformBuffersUpdatePackage{};
    uniformBuffersUpdatePackage.mainCamera = &m_mainCamera;
    uniformBuffersUpdatePackage.mainMeshQuaternion = m_mainModel.meshQuaternion;
    
    uniformBuffersUpdatePackage.swapchainImageExtent = displayDetails.swapchainImageExtent;
    uniformBuffersUpdatePackage.glfwWindow = displayDetails.glfwWindow;

    uniformBuffersUpdatePackage.mappedSceneUniformBufferMemory = m_scenePipelineComponents.mappedUniformBuffersMemory[m_currentFrame];
    uniformBuffersUpdatePackage.mappedSceneNormalsUniformBufferMemory = m_sceneNormalsPipelineComponents.mappedUniformBuffersMemory[m_currentFrame];
    uniformBuffersUpdatePackage.mappedCubemapUniformBufferMemory = m_cubemapPipelineComponents.mappedUniformBuffersMemory[m_currentFrame];
    uniformBuffersUpdatePackage.mappedDirectionalShadowUniformBufferMemory = m_directionalShadowOperation.pipelineComponents.mappedUniformBuffersMemory[m_currentFrame];
    uniformBuffersUpdatePackage.mappedPointShadowUniformBufferMemory = m_pointShadowOperation.pipelineComponents.mappedUniformBuffersMemory[m_currentFrame];
    
    Uniform::updateFrameUniformBuffers(uniformBuffersUpdatePackage);

    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // what semaphore to wait on and what stage of the pipeline to wait on.
    VkSemaphore waitSemaphores[] = {m_imageAvailibleSemaphores[m_currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &displayDetails.graphicsCommandBuffers[m_currentFrame];

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

    VkSwapchainKHR swapchains[] = {displayDetails.swapchain};
    presentationInfo.swapchainCount = 1;
    presentationInfo.pSwapchains = swapchains;
    presentationInfo.pImageIndices = &swapchainImageIndex;
    presentationInfo.pResults = nullptr;

    vkQueuePresentKHR(presentationQueue, &presentationInfo);

    
    m_currentFrame = (m_currentFrame + 1) % Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT;  // modulo operator to ensure frame index resets after MAX_FRAMES_IN_FLIGHT frames.
}

void RendererDetails::Renderer::setVulkanLogicalDevice(VkDevice *vulkanLogicalDevice)
{
    m_vulkanLogicalDevice = vulkanLogicalDevice;
}

void RendererDetails::Renderer::render(DisplayManager::DisplayDetails& displayDetails, uint32_t graphicsFamilyIndex, VkPhysicalDevice vulkanPhysicalDevice)
{
    DeviceHandler::VulkanDevices temporaryVulkanDevices{};
    temporaryVulkanDevices.physicalDevice = vulkanPhysicalDevice;
    temporaryVulkanDevices.logicalDevice = *m_vulkanLogicalDevice;


    Defaults::callbacksVariables.MAIN_CAMERA = &m_mainCamera;
    m_mainCamera.up = glm::vec3(0.0f, 1.0f, 0.0f);
    m_mainCamera.eye = glm::vec3(0.0f, 0.0f, -6.0f);
    m_mainCamera.center = glm::vec3(0.0f, 0.0f, 0.0f);
    m_mainCamera.swapchainImageExtent = displayDetails.swapchainImageExtent;

    fetchMaximumUsableSampleCount(vulkanPhysicalDevice, displayDetails.msaaSampleCount);
    
    createMemberRenderPass(displayDetails.swapchainImageFormat, displayDetails.msaaSampleCount, vulkanPhysicalDevice);
    

    VkDescriptorSetLayoutBinding cubemapUniformBufferLayoutBinding{};
    ResourceDescriptor::populateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, cubemapUniformBufferLayoutBinding);
    
    VkDescriptorSetLayoutBinding cubemapModelLayoutBinding{};
    ResourceDescriptor::populateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, cubemapModelLayoutBinding);

    std::vector<VkDescriptorSetLayoutBinding> cubemapDescriptorSetLayoutBindings = {cubemapUniformBufferLayoutBinding, cubemapModelLayoutBinding};
    ResourceDescriptor::createDescriptorSetLayout(cubemapDescriptorSetLayoutBindings, *m_vulkanLogicalDevice, m_cubemapPipelineComponents.descriptorSetLayout);
    
    createMemberCubemapPipeline(displayDetails.msaaSampleCount);
    
    VkDescriptorSetLayoutBinding sceneUniformBufferLayoutBinding{};
    ResourceDescriptor::populateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, (VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), sceneUniformBufferLayoutBinding);
    
    VkDescriptorSetLayoutBinding sceneMainModelAlbedoLayoutBinding{};  // main model albedo texture sampler.
    ResourceDescriptor::populateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, sceneMainModelAlbedoLayoutBinding);

    VkDescriptorSetLayoutBinding sceneMainModelNormalLayoutBinding{};  // main model normal map/texture sampler.
    ResourceDescriptor::populateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, sceneMainModelNormalLayoutBinding);

    VkDescriptorSetLayoutBinding sceneDirectionalShadowLayoutBinding{};
    ResourceDescriptor::populateDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, sceneDirectionalShadowLayoutBinding);

    VkDescriptorSetLayoutBinding scenePointShadowLayoutBinding{};
    ResourceDescriptor::populateDescriptorSetLayoutBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, scenePointShadowLayoutBinding);

    std::vector<VkDescriptorSetLayoutBinding> sceneDescriptorSetLayoutBindings = {sceneUniformBufferLayoutBinding, sceneMainModelAlbedoLayoutBinding, sceneMainModelNormalLayoutBinding, sceneDirectionalShadowLayoutBinding, scenePointShadowLayoutBinding};
    ResourceDescriptor::createDescriptorSetLayout(sceneDescriptorSetLayoutBindings, *m_vulkanLogicalDevice, m_scenePipelineComponents.descriptorSetLayout);
    
    createMemberScenePipeline(displayDetails.msaaSampleCount);

    
    VkDescriptorSetLayoutBinding sceneNormalsUniformBufferLayoutBinding{};
    ResourceDescriptor::populateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, (VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT), sceneNormalsUniformBufferLayoutBinding);
    
    std::vector<VkDescriptorSetLayoutBinding> sceneNormalsDescriptorSetLayoutBindings = {sceneNormalsUniformBufferLayoutBinding};
    ResourceDescriptor::createDescriptorSetLayout(sceneNormalsDescriptorSetLayoutBindings, *m_vulkanLogicalDevice, m_sceneNormalsPipelineComponents.descriptorSetLayout);
    
    createMemberSceneNormalsPipeline(displayDetails.msaaSampleCount);

    
    VkDescriptorSetLayoutBinding directionalShadowUniformBufferLayoutBinding{};
    ResourceDescriptor::populateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, (VK_SHADER_STAGE_VERTEX_BIT), directionalShadowUniformBufferLayoutBinding);
    
    std::vector<VkDescriptorSetLayoutBinding> directionalShadowDescriptorSetLayoutBindings = {directionalShadowUniformBufferLayoutBinding};
    ResourceDescriptor::createDescriptorSetLayout(directionalShadowDescriptorSetLayoutBindings, *m_vulkanLogicalDevice, m_directionalShadowOperation.pipelineComponents.descriptorSetLayout);

    
    VkDescriptorSetLayoutBinding pointShadowUniformBufferLayoutBinding{};
    ResourceDescriptor::populateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, (VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), pointShadowUniformBufferLayoutBinding);
    
    std::vector<VkDescriptorSetLayoutBinding> pointShadowDescriptorSetLayoutBindings = {pointShadowUniformBufferLayoutBinding};
    ResourceDescriptor::createDescriptorSetLayout(pointShadowDescriptorSetLayoutBindings, *m_vulkanLogicalDevice, m_pointShadowOperation.pipelineComponents.descriptorSetLayout);

    
    CommandManager::createGraphicsCommandPool(graphicsFamilyIndex, *m_vulkanLogicalDevice, displayDetails.graphicsCommandPool);

    Image::generateSwapchainImageDetails(displayDetails, temporaryVulkanDevices);

    SwapchainHandler::createSwapchainFramebuffers(displayDetails.swapchainImageViews, displayDetails.swapchainImageExtent, displayDetails.colorImageDetails.imageView, displayDetails.depthImageDetails.imageView, m_renderPass, *m_vulkanLogicalDevice, displayDetails.swapchainFramebuffers);

    m_directionalShadowOperation.generateMemberComponents((displayDetails.swapchainImageExtent.width / 1), (displayDetails.swapchainImageExtent.height / 1), 1, &RendererDetails::createShadowRenderPass, &RendererDetails::createDirectionalShadowPipeline, displayDetails.graphicsCommandPool, displayDetails.graphicsQueue, temporaryVulkanDevices);

    m_pointShadowOperation.generateMemberComponents((displayDetails.swapchainImageExtent.width / 1), (displayDetails.swapchainImageExtent.height / 1), 6, &RendererDetails::createShadowRenderPass, &RendererDetails::createPointShadowPipeline, displayDetails.graphicsCommandPool, displayDetails.graphicsQueue, temporaryVulkanDevices);

    m_mainModel.loadModelFromAbsolutePath((Defaults::applicationDefaults.SALAMANDER_ROOT_DIRECTORY + "/assets/models/Avocado/Avocado.gltf"));
    // TODO: add seperate "transfer" queue(see vulkan-tutorial page).
    m_mainModel.populateShaderBufferComponents(m_mainModel.meshVertices, displayDetails.graphicsCommandPool, displayDetails.graphicsQueue, temporaryVulkanDevices);
    Image::populateTextureDetails(m_mainModel.absoluteTextureImagePath, false, displayDetails.graphicsCommandPool, displayDetails.graphicsQueue, temporaryVulkanDevices, m_mainModel.textureDetails);
    Image::populateTextureDetails(m_mainModel.absoluteNormalImagePath, false, displayDetails.graphicsCommandPool, displayDetails.graphicsQueue, temporaryVulkanDevices, m_mainModel.normalTextureDetails);

    m_dummySceneNormalsModel.meshVertices = m_mainModel.meshVertices;
    m_dummySceneNormalsModel.meshIndices = m_mainModel.meshIndices;

    std::vector<ModelHandler::SceneNormalsVertexData> sceneNormalsVertexData;
    sceneNormalsVertexData.resize(m_mainModel.meshVertices.size());
    for (size_t i = 0; i < m_mainModel.meshVertices.size(); i += 1) {
        sceneNormalsVertexData[i] = {m_mainModel.meshVertices[i].position, m_mainModel.meshVertices[i].normal};
    }
    m_dummySceneNormalsModel.populateShaderBufferComponents(sceneNormalsVertexData, displayDetails.graphicsCommandPool, displayDetails.graphicsQueue, temporaryVulkanDevices);

    m_cubemapModel.loadModelFromAbsolutePath((Defaults::applicationDefaults.SALAMANDER_ROOT_DIRECTORY + "/assets/models/Cube/Cube.gltf"));
    
    std::vector<ModelHandler::CubemapVertexData> cubemapVertexData;
    cubemapVertexData.resize(m_cubemapModel.meshVertices.size());
    for (size_t i = 0; i < m_cubemapModel.meshVertices.size(); i += 1) {
        cubemapVertexData[i] = {m_cubemapModel.meshVertices[i].position};
    }
    m_cubemapModel.populateShaderBufferComponents(cubemapVertexData, displayDetails.graphicsCommandPool, displayDetails.graphicsQueue, temporaryVulkanDevices);

    m_dummyDirectionalShadowModel.meshVertices = m_mainModel.meshVertices;
    m_dummyDirectionalShadowModel.meshIndices = m_mainModel.meshIndices;

    m_dummyPointShadowModel.meshVertices = m_mainModel.meshVertices;
    m_dummyPointShadowModel.meshIndices = m_mainModel.meshIndices;
    
    std::vector<ModelHandler::ShadowVertexData> shadowVertexData;  // both the directional and point shadow operation use the same vertex data.
    shadowVertexData.resize(m_dummyDirectionalShadowModel.meshVertices.size());
    for (size_t i = 0; i < m_dummyDirectionalShadowModel.meshVertices.size(); i += 1) {
        shadowVertexData[i] = {m_dummyDirectionalShadowModel.meshVertices[i].position};
    }
    m_dummyDirectionalShadowModel.populateShaderBufferComponents(shadowVertexData, displayDetails.graphicsCommandPool, displayDetails.graphicsQueue, temporaryVulkanDevices);

    m_dummyPointShadowModel.populateShaderBufferComponents(shadowVertexData, displayDetails.graphicsCommandPool, displayDetails.graphicsQueue, temporaryVulkanDevices);

    Image::populateTextureDetails((Defaults::applicationDefaults.SALAMANDER_ROOT_DIRECTORY + "/assets/skyboxes/field"), true, displayDetails.graphicsCommandPool, displayDetails.graphicsQueue, temporaryVulkanDevices, m_cubemapModel.textureDetails);

    Uniform::createUniformBuffers(sizeof(Uniform::SceneUniformBufferObject), temporaryVulkanDevices, m_scenePipelineComponents.uniformBuffers, m_scenePipelineComponents.uniformBuffersMemory, m_scenePipelineComponents.mappedUniformBuffersMemory);
    ResourceDescriptor::createDescriptorPool(4, *m_vulkanLogicalDevice, m_scenePipelineComponents.descriptorPool);
    ResourceDescriptor::createDescriptorSets(m_scenePipelineComponents.descriptorSetLayout, m_scenePipelineComponents.descriptorPool, *m_vulkanLogicalDevice, m_scenePipelineComponents.descriptorSets);
    
    VkDescriptorImageInfo mainModelAlbedoDescriptorImageInfo{};
    ResourceDescriptor::populateDescriptorImageInfo(m_mainModel.textureDetails.textureSampler, m_mainModel.textureDetails.textureImageDetails.imageView, m_mainModel.textureDetails.textureImageDetails.imageLayout, mainModelAlbedoDescriptorImageInfo);
    VkWriteDescriptorSet mainModelAlbedoWriteDescriptorSet{};
    ResourceDescriptor::populateWriteDescriptorSet(nullptr, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &mainModelAlbedoDescriptorImageInfo, nullptr, mainModelAlbedoWriteDescriptorSet);

    VkDescriptorImageInfo mainModelNormalMapDescriptorImageInfo{};
    ResourceDescriptor::populateDescriptorImageInfo(m_mainModel.normalTextureDetails.textureSampler, m_mainModel.normalTextureDetails.textureImageDetails.imageView, m_mainModel.normalTextureDetails.textureImageDetails.imageLayout, mainModelNormalMapDescriptorImageInfo);
    VkWriteDescriptorSet mainModelNormalMapWriteDescriptorSet{};
    ResourceDescriptor::populateWriteDescriptorSet(nullptr, 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &mainModelNormalMapDescriptorImageInfo, nullptr, mainModelNormalMapWriteDescriptorSet);

    VkDescriptorImageInfo directionalShadowDescriptorImageInfo{};
    ResourceDescriptor::populateDescriptorImageInfo(m_directionalShadowOperation.depthTextureDetails.textureSampler, m_directionalShadowOperation.depthTextureDetails.textureImageDetails.imageView, m_directionalShadowOperation.depthTextureDetails.textureImageDetails.imageLayout, directionalShadowDescriptorImageInfo);
    VkWriteDescriptorSet directionalShadowWriteDescriptorSet{};
    ResourceDescriptor::populateWriteDescriptorSet(nullptr, 3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &directionalShadowDescriptorImageInfo, nullptr, directionalShadowWriteDescriptorSet);

    VkDescriptorImageInfo pointShadowDescriptorImageInfo{};
    ResourceDescriptor::populateDescriptorImageInfo(m_pointShadowOperation.depthTextureDetails.textureSampler, m_pointShadowOperation.depthTextureDetails.textureImageDetails.imageView, m_pointShadowOperation.depthTextureDetails.textureImageDetails.imageLayout, pointShadowDescriptorImageInfo);
    VkWriteDescriptorSet pointShadowWriteDescriptorSet{};
    ResourceDescriptor::populateWriteDescriptorSet(nullptr, 4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &pointShadowDescriptorImageInfo, nullptr, pointShadowWriteDescriptorSet);

    std::vector sceneWriteDescriptorSets = {mainModelAlbedoWriteDescriptorSet, mainModelNormalMapWriteDescriptorSet, directionalShadowWriteDescriptorSet, pointShadowWriteDescriptorSet};
    ResourceDescriptor::populateDescriptorSets(m_scenePipelineComponents.uniformBuffers, sceneWriteDescriptorSets, *m_vulkanLogicalDevice, m_scenePipelineComponents.descriptorSets);
    

    Uniform::createUniformBuffers(sizeof(Uniform::SceneNormalsUniformBufferObject), temporaryVulkanDevices, m_sceneNormalsPipelineComponents.uniformBuffers, m_sceneNormalsPipelineComponents.uniformBuffersMemory, m_sceneNormalsPipelineComponents.mappedUniformBuffersMemory);
    ResourceDescriptor::createDescriptorPool(0, *m_vulkanLogicalDevice, m_sceneNormalsPipelineComponents.descriptorPool);
    ResourceDescriptor::createDescriptorSets(m_sceneNormalsPipelineComponents.descriptorSetLayout, m_sceneNormalsPipelineComponents.descriptorPool, *m_vulkanLogicalDevice, m_sceneNormalsPipelineComponents.descriptorSets);
    
    std::vector<VkWriteDescriptorSet> sceneNormalsWriteDescriptorSets;
    ResourceDescriptor::populateDescriptorSets(m_sceneNormalsPipelineComponents.uniformBuffers, sceneNormalsWriteDescriptorSets, *m_vulkanLogicalDevice, m_sceneNormalsPipelineComponents.descriptorSets);
    

    Uniform::createUniformBuffers(sizeof(Uniform::CubemapUniformBufferObject), temporaryVulkanDevices, m_cubemapPipelineComponents.uniformBuffers, m_cubemapPipelineComponents.uniformBuffersMemory, m_cubemapPipelineComponents.mappedUniformBuffersMemory);
    ResourceDescriptor::createDescriptorPool(1, *m_vulkanLogicalDevice, m_cubemapPipelineComponents.descriptorPool);
    ResourceDescriptor::createDescriptorSets(m_cubemapPipelineComponents.descriptorSetLayout, m_cubemapPipelineComponents.descriptorPool, *m_vulkanLogicalDevice, m_cubemapPipelineComponents.descriptorSets);
    
    VkDescriptorImageInfo cubemapDescriptorImageInfo{};
    ResourceDescriptor::populateDescriptorImageInfo(m_cubemapModel.textureDetails.textureSampler, m_cubemapModel.textureDetails.textureImageDetails.imageView, m_cubemapModel.textureDetails.textureImageDetails.imageLayout, cubemapDescriptorImageInfo);
    VkWriteDescriptorSet cubemapWriteDescriptorSet{};
    ResourceDescriptor::populateWriteDescriptorSet(nullptr, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &cubemapDescriptorImageInfo, nullptr, cubemapWriteDescriptorSet);

    std::vector<VkWriteDescriptorSet> cubemapWriteDescriptorSets = {cubemapWriteDescriptorSet};
    ResourceDescriptor::populateDescriptorSets(m_cubemapPipelineComponents.uniformBuffers, cubemapWriteDescriptorSets, *m_vulkanLogicalDevice, m_cubemapPipelineComponents.descriptorSets);
    

    Uniform::createUniformBuffers(sizeof(Uniform::DirectionalShadowUniformBufferObject), temporaryVulkanDevices, m_directionalShadowOperation.pipelineComponents.uniformBuffers, m_directionalShadowOperation.pipelineComponents.uniformBuffersMemory, m_directionalShadowOperation.pipelineComponents.mappedUniformBuffersMemory);
    ResourceDescriptor::createDescriptorPool(0, *m_vulkanLogicalDevice, m_directionalShadowOperation.pipelineComponents.descriptorPool);
    ResourceDescriptor::createDescriptorSets(m_directionalShadowOperation.pipelineComponents.descriptorSetLayout, m_directionalShadowOperation.pipelineComponents.descriptorPool, *m_vulkanLogicalDevice, m_directionalShadowOperation.pipelineComponents.descriptorSets);

    std::vector<VkWriteDescriptorSet> directionalShadowWriteDescriptorSets;
    ResourceDescriptor::populateDescriptorSets(m_directionalShadowOperation.pipelineComponents.uniformBuffers, directionalShadowWriteDescriptorSets, *m_vulkanLogicalDevice, m_directionalShadowOperation.pipelineComponents.descriptorSets);


    Uniform::createUniformBuffers(sizeof(Uniform::PointShadowUniformBufferObject), temporaryVulkanDevices, m_pointShadowOperation.pipelineComponents.uniformBuffers, m_pointShadowOperation.pipelineComponents.uniformBuffersMemory, m_pointShadowOperation.pipelineComponents.mappedUniformBuffersMemory);
    ResourceDescriptor::createDescriptorPool(0, *m_vulkanLogicalDevice, m_pointShadowOperation.pipelineComponents.descriptorPool);
    ResourceDescriptor::createDescriptorSets(m_pointShadowOperation.pipelineComponents.descriptorSetLayout, m_pointShadowOperation.pipelineComponents.descriptorPool, *m_vulkanLogicalDevice, m_pointShadowOperation.pipelineComponents.descriptorSets);

    std::vector<VkWriteDescriptorSet> pointShadowWriteDescriptorSets;
    ResourceDescriptor::populateDescriptorSets(m_pointShadowOperation.pipelineComponents.uniformBuffers, pointShadowWriteDescriptorSets, *m_vulkanLogicalDevice, m_pointShadowOperation.pipelineComponents.descriptorSets);
    

    CommandManager::allocateChildCommandBuffers(displayDetails.graphicsCommandPool, Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT, *m_vulkanLogicalDevice, displayDetails.graphicsCommandBuffers);

    createMemberSynchronizationObjects();

    while (!glfwWindowShouldClose(displayDetails.glfwWindow)) {  // "main loop"
        DisplayManager::processWindowInput(displayDetails.glfwWindow);
        glfwPollEvents();

        drawFrame(displayDetails, vulkanPhysicalDevice, displayDetails.graphicsQueue, displayDetails.presentationQueue);
    }

    vkDeviceWaitIdle(*m_vulkanLogicalDevice);  // wait for the logical device to finish all operations before termination.
}

void RendererDetails::Renderer::cleanupRenderer()
{
    m_mainModel.cleanupModel(false, *m_vulkanLogicalDevice);
    m_dummySceneNormalsModel.cleanupModel(true, *m_vulkanLogicalDevice);
    m_cubemapModel.cleanupModel(false, *m_vulkanLogicalDevice);
    m_dummyDirectionalShadowModel.cleanupModel(true, *m_vulkanLogicalDevice);
    m_dummyPointShadowModel.cleanupModel(true, *m_vulkanLogicalDevice);
    
    for (size_t i = 0; i < Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT; i += 1) {
        vkDestroySemaphore(*m_vulkanLogicalDevice, m_imageAvailibleSemaphores[i], nullptr);
        vkDestroySemaphore(*m_vulkanLogicalDevice, m_renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(*m_vulkanLogicalDevice, m_inFlightFences[i], nullptr);
    }

    m_cubemapPipelineComponents.cleanupPipelineComponents(*m_vulkanLogicalDevice);
    m_scenePipelineComponents.cleanupPipelineComponents(*m_vulkanLogicalDevice);
    m_sceneNormalsPipelineComponents.cleanupPipelineComponents(*m_vulkanLogicalDevice);
    
    m_directionalShadowOperation.cleanupOffscreenOperation(*m_vulkanLogicalDevice);
    m_pointShadowOperation.cleanupOffscreenOperation(*m_vulkanLogicalDevice);

    vkDestroyRenderPass(*m_vulkanLogicalDevice, m_renderPass, nullptr);
}

RendererDetails::Renderer::Renderer()
{
    
}
