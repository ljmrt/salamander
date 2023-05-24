#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <core/DisplayManager/DisplayManager.h>
#include <core/Defaults/Defaults.h>
#include <core/Logging/ErrorLogger.h>

#include <string>


void DisplayManager::initializeGLFW()
{
    glfwInit();
    // to be expanded.
}

void DisplayManager::createWindow(size_t windowWidth, size_t windowHeight, std::string windowName, GLFWwindow*& resultWindow)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // don't create an OpenGL context.
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    resultWindow = glfwCreateWindow(windowWidth, windowHeight, windowName.c_str(), nullptr, nullptr);
}

void DisplayManager::createWindowSurface(VkInstance vkInstance, GLFWwindow *glfwWindow, VkSurfaceKHR& resultWindowSurface)
{
    VkResult windowSurfaceCreationResult = glfwCreateWindowSurface(vkInstance, glfwWindow, nullptr, &resultWindowSurface);
    if (windowSurfaceCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create window surface");
    }
}

void DisplayManager::processWindowInput(GLFWwindow *glfwWindow)
{
    if (GLFW_PRESS == glfwGetKey(glfwWindow, GLFW_KEY_X)) {  // close window on X press.
        glfwSetWindowShouldClose(glfwWindow, true);
    }
    if (GLFW_PRESS == glfwGetKey(glfwWindow, GLFW_KEY_R)) {  // reset model orientation on R press.
        Defaults::callbacksVariables.MAIN_CAMERA->baseQuaternion = glm::identity<glm::quat>();
    }
}

void DisplayManager::cleanupGLFW(GLFWwindow *glfwWindow)
{
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}
