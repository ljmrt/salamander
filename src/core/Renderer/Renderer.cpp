#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Renderer/Renderer.h>
#include <core/VulkanInstance/VulkanInstance.h>

#include <cstdint>
#include <iostream>


void renderer::run()
{
    windowInit();
    vulkanInit();
    render();
    cleanup();
}

void renderer::windowInit()
{
    glfwInit();
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // don't create an OpenGL context.
    // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);
}

void renderer::vulkanInit()
{
    m_vkInstance = VulkanInstance(WINDOW_NAME);
}

void renderer::render()
{
    while (!glfwWindowShouldClose(m_window)) {
        renderProcessInput();
        glfwPollEvents();
    }
}

void renderer::renderProcessInput()
{
    if (GLFW_PRESS == glfwGetKey(m_window, GLFW_KEY_X)) {
        glfwSetWindowShouldClose(m_window, true);
    }
}

void renderer::cleanup()
{
    vkDestroyInstance(m_vkInstance.m_instance, nullptr);
    
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
