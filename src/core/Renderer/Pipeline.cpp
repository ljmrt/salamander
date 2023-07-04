#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Renderer/Pipeline.h>
#include <core/Renderer/Renderer.h>
#include <core/Shader/Shader.h>
#include <core/Shader/ResourceDescriptor.h>
#include <core/Model/ModelHandler.h>
#include <core/Logging/ErrorLogger.h>
#include <core/Defaults/Defaults.h>

#include <string.h>


void Pipeline::PipelineComponents::createMemberPipeline(Pipeline::PipelineData& pipelineData)
{
    std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
    if (strcmp(pipelineData.vertexShaderBytecodeAbsolutePath.c_str(), "*NA*") != 0) {  // if the vertex shader is enabled(absolute path not "*NA*").
        Shader::createShader(pipelineData.vertexShaderBytecodeAbsolutePath, VK_SHADER_STAGE_VERTEX_BIT, pipelineData.vulkanLogicalDevice, this->pipelineShaders.vertexShader);
        shaderStageCreateInfos.push_back(this->pipelineShaders.vertexShader.shaderStageCreateInfo);
    }
    if (strcmp(pipelineData.geometryShaderBytecodeAbsolutePath.c_str(), "*NA*") != 0) {  // if the geometry shader is enabled(absolute path not "*NA*").
        Shader::createShader(pipelineData.geometryShaderBytecodeAbsolutePath, VK_SHADER_STAGE_GEOMETRY_BIT, pipelineData.vulkanLogicalDevice, this->pipelineShaders.geometryShader);
        shaderStageCreateInfos.push_back(this->pipelineShaders.geometryShader.shaderStageCreateInfo);
    }
    if (strcmp(pipelineData.fragmentShaderBytecodeAbsolutePath.c_str(), "*NA*") != 0) {  // if the fragment shader is enabled(absolute path not "*NA*").
        Shader::createShader(pipelineData.fragmentShaderBytecodeAbsolutePath, VK_SHADER_STAGE_FRAGMENT_BIT, pipelineData.vulkanLogicalDevice, this->pipelineShaders.fragmentShader);
        shaderStageCreateInfos.push_back(this->pipelineShaders.fragmentShader.shaderStageCreateInfo);
    }


    // allocate heap memory to preserve these components(necessary).
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;  // vector "header" is on the stack, elements are on the heap, which works for our case.
    VkVertexInputBindingDescription* bindingDescriptionPointer = new VkVertexInputBindingDescription;
    Defaults::applicationCleanup.vertexInputBindingDescriptionsMemory.push_back(bindingDescriptionPointer);
    
    ResourceDescriptor::populateBindingDescription(pipelineData.vertexDataStride, *bindingDescriptionPointer);
    pipelineData.fetchAttributeDescriptions(attributeDescriptions);  // use the custom attribute descriptions fetch function.

    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
    ModelHandler::populateVertexInputCreateInfo(attributeDescriptions, bindingDescriptionPointer, vertexInputCreateInfo);


    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
    ModelHandler::populateInputAssemblyCreateInfo(pipelineData.inputAssemblyTopology, pipelineData.inputAssemblyPrimitiveRestartEnable, inputAssemblyCreateInfo);


    VkPipelineViewportStateCreateInfo viewportCreateInfo{};
    RendererDetails::populateViewportCreateInfo(pipelineData.viewportViewportCount, pipelineData.viewportScissorCount, viewportCreateInfo);


    VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo{};
    RendererDetails::populateRasterizationCreateInfo(pipelineData.rasterizationCullMode, pipelineData.rasterizationFrontFace, rasterizationCreateInfo);


    VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo{};
    RendererDetails::populateMultisamplingCreateInfo(pipelineData.multisamplingRasterizationSamples, pipelineData.multisamplingMinSampleShading, multisamplingCreateInfo);


    VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo{};
    RendererDetails::populateDepthStencilCreateInfo(pipelineData.depthStencilDepthTestEnable, pipelineData.depthStencilDepthWriteEnable, pipelineData.depthStencilDepthCompareOp, depthStencilCreateInfo);

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo{};
    RendererDetails::populateColorBlendComponents(pipelineData.colorBlendColorWriteMask, pipelineData.colorBlendBlendEnable, colorBlendAttachment, colorBlendCreateInfo);


    VkPipelineDynamicStateCreateInfo dynamicStatesCreateInfo{};
    RendererDetails::populateDynamicStatesCreateInfo(pipelineData.dynamicStatesDynamicStates, dynamicStatesCreateInfo);


    RendererDetails::createPipelineLayout(pipelineData.vulkanLogicalDevice, this->descriptorSetLayout, pipelineData.pushConstant, this->pipelineLayout);


    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStageCreateInfos.size());
    pipelineCreateInfo.pStages = shaderStageCreateInfos.data();

    pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
    pipelineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
    pipelineCreateInfo.pDepthStencilState = &depthStencilCreateInfo;
    pipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
    pipelineCreateInfo.pDynamicState = &dynamicStatesCreateInfo;

    pipelineCreateInfo.layout = this->pipelineLayout;
    pipelineCreateInfo.renderPass = pipelineData.pipelineRenderPass;
    pipelineCreateInfo.subpass = 0;

    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = -1;

    size_t pipelineCreationResult = vkCreateGraphicsPipelines(pipelineData.vulkanLogicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &this->pipeline);
    if (pipelineCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create a graphics pipeline.");
    }

    if (strcmp(pipelineData.vertexShaderBytecodeAbsolutePath.c_str(), "*NA*") != 0) {  // if the vertex shader is enabled(absolute path not "*NA*").
        vkDestroyShaderModule(pipelineData.vulkanLogicalDevice, this->pipelineShaders.fragmentShader.shaderModule, nullptr);
    }
    if (strcmp(pipelineData.geometryShaderBytecodeAbsolutePath.c_str(), "*NA*") != 0) {  // if the geometry shader is enabled(absolute path not "*NA*").
        vkDestroyShaderModule(pipelineData.vulkanLogicalDevice, this->pipelineShaders.geometryShader.shaderModule, nullptr);
    }
    if (strcmp(pipelineData.fragmentShaderBytecodeAbsolutePath.c_str(), "*NA*") != 0) {  // if the fragment shader is enabled(absolute path not "*NA*").
        vkDestroyShaderModule(pipelineData.vulkanLogicalDevice, this->pipelineShaders.vertexShader.shaderModule, nullptr);
    }
}
        
void Pipeline::PipelineComponents::cleanupPipelineComponents(VkDevice vulkanLogicalDevice)
{
    for (size_t i = 0; i < this->uniformBuffers.size(); i += 1) {
        vkDestroyBuffer(vulkanLogicalDevice, this->uniformBuffers[i], nullptr);
        vkFreeMemory(vulkanLogicalDevice, this->uniformBuffersMemory[i], nullptr);        
    }
    
    vkDestroyDescriptorPool(vulkanLogicalDevice, this->descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(vulkanLogicalDevice, this->descriptorSetLayout, nullptr);
    
    vkDestroyPipeline(vulkanLogicalDevice, this->pipeline, nullptr);
    vkDestroyPipelineLayout(vulkanLogicalDevice, this->pipelineLayout, nullptr);
}
