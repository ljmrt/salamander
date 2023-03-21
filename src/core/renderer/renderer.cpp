#include <core/renderer/renderer.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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
    vulkanCreateInstance();
}

void renderer::vulkanCreateInstance()
{
    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = WINDOW_NAME;
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName = "No Engine";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &applicationInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    createInfo.enabledLayerCount = 0;

    VkResult instanceResult = vkCreateInstance(&createInfo, nullptr, &m_instance);
    if (instanceResult != VK_SUCCESS) {
        throw std::runtime_error("failed to create vulkan instance!");
    }
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
    vkDestroyInstance(m_instance, nullptr);
    
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
