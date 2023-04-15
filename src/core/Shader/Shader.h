#ifndef SHADER_H
#define SHADER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <vector>


namespace Shader
{
    struct Shader {
        std::string bytecodeFilePath;
        VkShaderModule shaderModule;
        VkPipelineShaderStageCreateInfo shaderStageCreateInfo{};
    };

    struct PipelineShaders {
        Shader vertexShader;
        Shader fragmentShader;
    };
    
    // create a shader module using the supplied bytecode.
    //
    // @param shaderBytecode the shader's bytecode in a vector of chars.
    // @param vulkanLogicalDevice Vulkan instance's logical device.
    // @param resultShaderModule stored created shader module.
    void createShaderModule(std::vector<char> shaderBytecode, VkDevice vulkanLogicalDevice, VkShaderModule& resultShaderModule);

    // complete data within a Shader struct(fill out shader module and shader shade create info).
    //
    // bytecodeFilePath must have a valid value.
    //
    // @param shaderStage bitmask of shader stage(ex: VK_SHADER_STAGE_VERTEX_BIT).
    // @param vulkanLogicalDevice Vulkan instance's logical device.
    // @param shader shader to complete.
    void completeShaderData(VkShaderStageFlagBits shaderStage, VkDevice vulkanLogicalDevice, Shader& shader);

    // wrapper function to simplify shader creation.
    //
    // sets shaderFilePath and then calls completeShaderData.
    //
    // @param bytecodeFilePath file path of the shader to create.
    // @param shaderStage Vulkan shader stage bitmask of the shader's stage.
    // @param vulkanLogicalDevice Vulkan instance's logical device.
    // @param shader stored created shader.
    void createShader(std::string bytecodeFilePath, VkShaderStageFlagBits shaderStage, VkDevice vulkanLogicalDevice, Shader& shader);
}

#endif  // SHADER_H
