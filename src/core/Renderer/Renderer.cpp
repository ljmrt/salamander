#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Renderer/Renderer.h>
#include <core/DisplayManager/DisplayManager.h>
#include <core/VulkanInstance/VulkanInstance.h>
#include <core/Logging/ErrorLogger.h>
#include <core/Defaults/Defaults.h>
#include <utils/FileUtils.h>

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>


Renderer::Renderer()
{
    
}

void Renderer::run()
{
    DisplayManager::initializeGLFW();
    DisplayManager::createWindow(Defaults::windowDefaults.MAIN_WINDOW_WIDTH, Defaults::windowDefaults.MAIN_WINDOW_HEIGHT, Defaults::windowDefaults.MAIN_WINDOW_NAME, m_instance.m_displayDetails.glfwWindow);
    
    Renderer::vulkanInit();
    
    Renderer::render();
    
    Renderer::cleanup();
}

void Renderer::vulkanInit()
{
    m_instance = VulkanInstance(Defaults::windowDefaults.MAIN_WINDOW_NAME, m_instance.m_displayDetails.glfwWindow);
}

void Renderer::render()
{
    DisplayManager::stallWindow(m_instance.m_displayDetails.glfwWindow);
}

void Renderer::createGraphicsPipeline()
{
    // TODO: extract code into multiple methods(create shader stage info, setup shader stages).
    std::vector<char> vertexShaderBytecode;
    FileUtils::readFileChars("/home/lucas/programming/graphics/salamander-engine/build/triangle.vs", vertexShaderBytecode);
    
    VkShaderModule vertexShaderModule;
    Renderer::createShaderModule(vertexShaderBytecode, vertexShaderModule);

    VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo{};
    vertexShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderStageCreateInfo.module = vertexShaderModule;
    vertexShaderStageCreateInfo.pName = "main";  // shader entry point.
    
    
    std::vector<char> fragmentShaderBytecode;
    FileUtils::readFileChars("/home/lucas/programming/graphics/salamander-engine/build/triangle.fs", fragmentShaderBytecode);
    
    VkShaderModule fragmentShaderModule;
    Renderer::createShaderModule(fragmentShaderBytecode, fragmentShaderModule);
    
    VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo{};
    fragmentShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderStageCreateInfo.module = vertexShaderModule;
    fragmentShaderStageCreateInfo.pName = "main";  // shader entry point.


    VkPipelineShaderStageCreateInfo shaderStages[] = {vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo};
}

void Renderer::createShaderModule(const std::vector<char> shaderBytecode, VkShaderModule& resultShaderModule)
{
    VkShaderModuleCreateInfo shaderModuleCreateInfo{};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize = shaderBytecode.size();  // somehow not incorrect?
    shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t *>(shaderBytecode.data());  // Vulkan requires the bytecode pointer to be a uint32_t.

    uint32_t shaderModuleCreationResult = vkCreateShaderModule(m_instance.m_physicalDevice, &shaderModuleCreateInfo, nullptr, &resultShaderModule);
    if (shaderModuleCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create shader module.");
    }
}

void Renderer::cleanup()
{
    m_instance.cleanupInstance();

    vkDestroyShaderModule(m_instance.m_physicalDevice, m_fragmentShaderModule, nullptr);
    vkDestroyShaderModule(m_instance.m_physicalDevice, m_vertexShaderModule, nullptr);
    
    DisplayManager::cleanupGLFW(m_instance.m_displayDetails.glfwWindow);
}
