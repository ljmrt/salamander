#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Renderer/Renderer.h>
#include <core/VulkanInstance/VulkanInstance.h>
#include <core/VulkanInstance/supportUtils.h>
#include <core/VulkanExtensions/VulkanExtensions.h>

#include <cstdint>
#include <iostream>


void Renderer::run()
{
    windowInit();
    vulkanInit();
    render();
    cleanup();
}

void Renderer::windowInit()
{
    glfwInit();
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // don't create an OpenGL context.
    // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);
}

void Renderer::vulkanInit()
{
    m_instance = VulkanInstance(WINDOW_NAME);
}

void Renderer::render()
{
    while (!glfwWindowShouldClose(m_window)) {
        renderProcessInput();
        glfwPollEvents();
    }
}

void Renderer::renderProcessInput()
{
    if (GLFW_PRESS == glfwGetKey(m_window, GLFW_KEY_X)) {
        glfwSetWindowShouldClose(m_window, true);
    }
}

void Renderer::cleanup()
{
    if (supportUtils::DEBUG_ENABLED) {
        VulkanExtensions::DestroyDebugUtilsMessengerEXT(m_instance.vkInstance, m_instance.debugMessenger, nullptr);
    }
    
    vkDestroyInstance(m_instance.vkInstance, nullptr);
    
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
