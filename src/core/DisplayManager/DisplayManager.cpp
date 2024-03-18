#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

#include <core/DisplayManager/DisplayManager.h>
#include <core/DisplayManager/SwapchainHandler.h>
#include <core/Defaults/Defaults.h>
#include <core/Logging/ErrorLogger.h>

#include <string>


void DisplayManager::DisplayDetails::cleanupDisplayDetails(VkDevice vulkanLogicalDevice, bool preserveCommandPool)
{
    SwapchainHandler::cleanupSwapchain(*this, vulkanLogicalDevice);
    
    this->depthImageDetails.cleanupImageDetails(vulkanLogicalDevice);
    this->colorImageDetails.cleanupImageDetails(vulkanLogicalDevice);

    if (preserveCommandPool == false) {
        vkDestroyCommandPool(vulkanLogicalDevice, this->graphicsCommandPool, nullptr);  // child command buffers automatically freed.    
    }

}

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
    if (GLFW_PRESS == glfwGetKey(glfwWindow, GLFW_KEY_ESCAPE)) {  // close window on "escape" key press.
        glfwSetWindowShouldClose(glfwWindow, true);
    }
    if (GLFW_PRESS == glfwGetKey(glfwWindow, GLFW_KEY_R)) {  // reset model orientation on "R" key press.
        Defaults::callbacksVariables.MAIN_CAMERA->baseQuaternion = glm::identity<glm::quat>();
    }
}

void DisplayManager::cleanupGLFW(GLFWwindow *glfwWindow)
{
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}
