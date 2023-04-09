#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Shader/Shader.h>
#include <core/Logging/ErrorLogger.h>
#include <utils/FileUtils.h>

#include <vector>


void Shader::createShaderModule(const std::vector<char> shaderBytecode, VkDevice logicalDevice, VkShaderModule& resultShaderModule)
{
    VkShaderModuleCreateInfo shaderModuleCreateInfo{};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize = shaderBytecode.size();  // somehow not incorrect?
    shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t *>(shaderBytecode.data());  // Vulkan requires the bytecode pointer to be a uint32_t.

    uint32_t shaderModuleCreationResult = vkCreateShaderModule(logicalDevice, &shaderModuleCreateInfo, nullptr, &resultShaderModule);
    if (shaderModuleCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create shader module.");
    }
}

void Shader::completeShaderData(VkShaderStageFlagBits shaderStage, VkDevice logicalDevice, Shader& shader)
{
    std::vector<char> shaderBytecode;
    FileUtils::readFileChars(shader.shaderFilePath, true, shaderBytecode);
    createShaderModule(shaderBytecode, logicalDevice, shader.shaderModule);

    shader.shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader.shaderStageCreateInfo.stage = shaderStage;
    shader.shaderStageCreateInfo.module = shader.shaderModule;
    shader.shaderStageCreateInfo.pName = "main";  // shader entry point.
}
