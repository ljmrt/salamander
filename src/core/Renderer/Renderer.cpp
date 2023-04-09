#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Renderer/Renderer.h>
#include <core/Shader/Shader.h>
#include <core/DisplayManager/DisplayManager.h>
#include <core/VulkanInstance/VulkanInstance.h>
#include <core/Logging/ErrorLogger.h>
#include <core/Defaults/Defaults.h>

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
    m_shaderStages.vertexShader.shaderFilePath = "/home/lucas/programming/graphics/salamander-engine/include/shaders/triangle.vs";
    Shader::completeShaderData(VK_SHADER_STAGE_VERTEX_BIT, m_instance.m_logicalDevice, m_shaderStages.vertexShader);

    m_shaderStages.fragmentShader.shaderFilePath = "/home/lucas/programming/graphics/salamander-engine/include/shaders/triangle.fs";
    Shader::completeShaderData(VK_SHADER_STAGE_FRAGMENT_BIT, m_instance.m_logicalDevice, m_shaderStages.fragmentShader);

    // VkPipelineShaderStageCreateInfo shaderStages[] = {m_shaderStages.vertexShader.shaderStageCreateInfo, m_shaderStages.fragmentShader.shaderStageCreateInfo};
}

void Renderer::cleanup()
{
    m_instance.cleanupInstance();

    vkDestroyShaderModule(m_instance.m_logicalDevice, m_shaderStages.fragmentShader.shaderModule, nullptr);
    vkDestroyShaderModule(m_instance.m_logicalDevice, m_shaderStages.vertexShader.shaderModule, nullptr);
    
    DisplayManager::cleanupGLFW(m_instance.m_displayDetails.glfwWindow);
}
