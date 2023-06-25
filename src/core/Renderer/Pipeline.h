#ifndef PIPELINE_H
#define PIPELINE_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Shader/Shader.h>

#include <vector>


namespace Pipeline
{
    struct PipelineData {  // all the data necessary for a pipeline creation(as well as pipeline layout and pipeline shaders).
        VkDevice vulkanLogicalDevice

        // shader data.
        // set any shader bytecode paths as *NA* to disable the shader.
        std::string vertexShaderBytecodeAbsolutePath;
        std::string geometryShaderBytecodeAbsolutePath;
        std::string fragmentShaderBytecodeAbsolutePath;

        // binding/attribute description data.
        uint32_t vertexDataStride;  // the stride for what vertex data will be passed in(see ModelHandler vertex data structs).
        std::function<void(std::vector<VkVertexInputAttributeDescription>&)> fetchAttributeDescriptions;  // a function that fetches the attribute descriptions for a pipeline.

        // input assembly data.
        VkPrimitiveTopology inputAssemblyTopology;
        VkBool32 inputAssemblyPrimitiveRestartEnable;

        // viewport data.
        uint32_t viewportViewportCount;  // redundant name, but best to be consistent.
        uint32_t viewportScissorCount;

        // rasterization data.
        VkCullModeFlags rasterizationCullMode;
        VkFrontFace rasterizationFrontFace;

        // multisampling data.
        VkSampleCountFlagBits multisamplingRasterizationSamples;
        float multisamplingMinSampleShading;

        // depth stencil data.
        VkBool32 depthStencilDepthTestEnable;
        VkBool32 depthStencilDepthWriteEnable;
        VkCompareOp depthStencilDepthCompareOp;

        // color blend data.
        VkColorComponentFlags colorBlendColorWriteMask;
        VkBool32 colorBlendBlendEnable;

        // dynamic states data.
        std::vector<VkDynamicState> dynamicStatesDynamicStates;

        // misc. pipeline data.
        VkRenderPass pipelineRenderPass;
    };
    
    struct PipelineComponents {        
        VkDescriptorSetLayout descriptorSetLayout;  // this pipeline layout's descriptor set layout.
        VkDescriptorPool descriptorPool;  // the descriptor pool to use in scene descriptor set creation.
        std::vector<VkDescriptorSet> descriptorSets;  // uniform buffer descriptor sets.

        std::vector<VkBuffer> uniformBuffers;  // the uniform buffers specified for the pipeline.
        std::vector<VkDeviceMemory> uniformBuffersMemory;  // the uniform buffers' memory.
        std::vector<void *> mappedUniformBuffersMemory;  // the mapped memory of the uniform buffers.

        VkPipelineLayout pipelineLayout;  // this pipeline's pipeline layout.
        VkPipeline pipeline;  // the pipeline.

        Shader::PipelineShaders pipelineShaders;  // pipeline shaders/shader stages.


        // create member pipeline, pipeline layout, and pipeline shaders from the provided pipeline data.
        void createMemberPipeline(Pipeline::PipelineData& pipelineData);
        
        // cleanup the pipeline components.
        //
        // @param vulkanLogicalDevice Vulkan logical device used in pipeline component cleanup.
        void cleanupPipelineComponents(VkDevice vulkanLogicalDevice);
    };
}


#endif  // PIPELINE_H
