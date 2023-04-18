#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/DisplayManager/DisplayManager.h>
#include <core/Logging/ErrorLogger.h>

#include <string>


void DisplayManager::initializeGLFW()
{
    glfwInit();
    // to be expanded.
}

void DisplayManager::createWindow(uint32_t windowWidth, uint32_t windowHeight, std::string windowName, GLFWwindow*& resultWindow)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // don't create an OpenGL context.
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    resultWindow = glfwCreateWindow(windowWidth, windowHeight, windowName.c_str(), nullptr, nullptr);
}

void DisplayManager::createWindowSurface(VkInstance vkInstance, GLFWwindow *glfwWindow, VkSurfaceKHR& resultWindowSurface)
{
    int creationResult = glfwCreateWindowSurface(vkInstance, glfwWindow, nullptr, &resultWindowSurface);
    if (creationResult != VK_SUCCESS) {
        throwDebugException("Failed to create window surface");
    }
}

void DisplayManager::processWindowInput(GLFWwindow *glfwWindow)
{
    if (GLFW_PRESS == glfwGetKey(glfwWindow, GLFW_KEY_X)) {
        glfwSetWindowShouldClose(glfwWindow, true);
    }
}

void DisplayManager::cleanupGLFW(GLFWwindow *glfwWindow)
{
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}
