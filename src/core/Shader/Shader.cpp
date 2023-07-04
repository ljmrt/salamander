#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Shader/Shader.h>
#include <core/Logging/ErrorLogger.h>
#include <utils/FileUtils.h>

#include <vector>

glm::mat4 Shader::shadowTransforms[6];

void Shader::createShaderModule(std::vector<char> shaderBytecode, VkDevice vulkanLogicalDevice, VkShaderModule& createdShaderModule)
{
    VkShaderModuleCreateInfo shaderModuleCreateInfo{};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    
    shaderModuleCreateInfo.codeSize = shaderBytecode.size();
    shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t *>(shaderBytecode.data());  // Vulkan requires the bytecode pointer to be a uint32_t.

    uint32_t shaderModuleCreationResult = vkCreateShaderModule(vulkanLogicalDevice, &shaderModuleCreateInfo, nullptr, &createdShaderModule);
    if (shaderModuleCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create shader module.");
    }
}

void Shader::completeShaderData(VkShaderStageFlagBits shaderStage, VkDevice vulkanLogicalDevice, Shader& incompleteShader)
{
    std::vector<char> shaderBytecode;
    FileUtils::readFileChars(incompleteShader.bytecodeFilePath.c_str(), true, shaderBytecode);
    createShaderModule(shaderBytecode, vulkanLogicalDevice, incompleteShader.shaderModule);

    
    incompleteShader.shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    
    incompleteShader.shaderStageCreateInfo.stage = shaderStage;
    incompleteShader.shaderStageCreateInfo.module = incompleteShader.shaderModule;
    incompleteShader.shaderStageCreateInfo.pName = "main";  // shader entry point.
    
    incompleteShader.shaderStageCreateInfo.flags = 0;
    incompleteShader.shaderStageCreateInfo.pNext = nullptr;
    incompleteShader.shaderStageCreateInfo.pSpecializationInfo = nullptr;
}

void Shader::createShader(std::string bytecodeFilePath, VkShaderStageFlagBits shaderStage, VkDevice vulkanLogicalDevice, Shader& createdShader)
{
    createdShader.bytecodeFilePath = bytecodeFilePath;
    completeShaderData(shaderStage, vulkanLogicalDevice, createdShader);
}
