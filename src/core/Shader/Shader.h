#ifndef SHADER_H
#define SHADER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>


namespace Shader
{
    struct Shader {
        const char *shaderFilePath;
        VkShaderModule shaderModule;
        VkPipelineShaderStageCreateInfo shaderStageCreateInfo;
    };

    struct ShaderStages {
        Shader vertexShader;
        Shader fragmentShader;
    };
    
    // create a shader module using the supplied bytecode.
    //
    // @param shaderBytecode the shader's bytecode in a vector of chars.
    // @param logicalDevice Vulkan instance's logical device.
    // @param resultShaderModule stored created shader module.
    void createShaderModule(const std::vector<char> shaderBytecode, VkDevice logicalDevice, VkShaderModule& resultShaderModule);

    // complete data within a Shader struct(fill out shader module and shader shade create info).
    //
    // shaderFilePath must have a valid value.
    //
    // @param shaderStage bitmask of shader stage(ex: VK_SHADER_STAGE_VERTEX_BIT).
    // @param logicalDevice Vulkan instance's logical device.
    // @param shader shader to complete.
    void completeShaderData(VkShaderStageFlagBits shaderStage, VkDevice logicalDevice, Shader& shader);
}

#endif  // SHADER_H
