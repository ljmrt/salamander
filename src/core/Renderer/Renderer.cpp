#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Renderer/Renderer.h>
#include <core/DisplayManager/DisplayManager.h>
#include <core/VulkanInstance/VulkanInstance.h>
#include <core/Defaults/Defaults.h>

#include <cstdint>
#include <iostream>
#include <string>


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

void Renderer::cleanup()
{
    m_instance.cleanupInstance();
    DisplayManager::cleanupGLFW(m_instance.m_displayDetails.glfwWindow);
}
