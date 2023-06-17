#ifndef PIPELINECOMPONENTS_H
#define PIPELINECOMPONENTS_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Shader/Shader.h>

#include <vector>


namespace RendererDetails
{
    // TODO: abstract various component creation/usage into methods and allow for disabling(e.g scene normals).
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

        
        void cleanupPipelineComponents(VkDevice vulkanLogicalDevice);
    };
}


#endif  // PIPELINECOMPONENTS_H
