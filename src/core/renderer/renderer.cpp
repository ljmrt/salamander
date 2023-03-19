#include <core/renderer/renderer.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdint>


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

}

void renderer::render()
{
    while (!glfwWindowShouldClose(m_window)) {
        render_process_input();
        glfwPollEvents();
    }
}

void renderer::render_process_input()
{
    if (GLFW_PRESS == glfwGetKey(m_window, GLFW_KEY_X)) {
        glfwSetWindowShouldClose(m_window, true);
    }
}

void renderer::cleanup()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
