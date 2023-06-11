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
        Shader geometryShader;  // optional.
        Shader fragmentShader;
    };
    
    
    // create a shader module using the supplied bytecode.
    //
    // @param shaderBytecode the shader's bytecode in a vector of chars.
    // @param vulkanLogicalDevice Vulkan instance's logical device.
    // @param createdShaderModule stored created shader module.
    void createShaderModule(std::vector<char> shaderBytecode, VkDevice vulkanLogicalDevice, VkShaderModule& createdShaderModule);

    // complete data within a Shader struct(fill out shader module and shader shade create info).
    //
    // bytecodeFilePath must have a valid value.
    //
    // @param shaderStage bitmask of shader stage(ex: VK_SHADER_STAGE_VERTEX_BIT).
    // @param vulkanLogicalDevice the Vulkan instance's logical device.
    // @param incompleteShader incomplete shader to complete.
    void completeShaderData(VkShaderStageFlagBits shaderStage, VkDevice vulkanLogicalDevice, Shader& incompleteShader);

    // wrapper function to simplify shader creation.
    //
    // sets shaderFilePath and then calls completeShaderData.
    //
    // @param bytecodeFilePath file path of the shader to create.
    // @param shaderStage Vulkan shader stage bitmask of the shader's stage.
    // @param vulkanLogicalDevice Vulkan instance's logical device.
    // @param createdShader stored created shader.
    void createShader(std::string bytecodeFilePath, VkShaderStageFlagBits shaderStage, VkDevice vulkanLogicalDevice, Shader& createdShader);
}

#endif  // SHADER_H
