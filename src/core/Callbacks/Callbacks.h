#ifndef CALLBACKS_H
#define CALLBACKS_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


namespace Callbacks
{
    // debug messenger callback function.
    //
    // handles and prints debug messages.
    //
    // @param messageSeverity the severity of the message(VK_DEBUG_UTILS_MESSAGE_SEVERITY_[VERBOSE|INFO|WARNING|ERROR]_BIT_EXT).
    // @param messageType the type of the message(VK_DEBUG_UTILS_MESSAGE_TYPE_[GENERAL|VALIDATION|PERFORMANCE]_BIT_EXT).
    // @param pCallbackData VkDebugUtilsMessengerCallbackDataEXT struct containing message details.
    // @param pUserData pointer specified during callback setup.
    // @return indication of causing Vulkan call abortion(always VK_FALSE for our case).
    VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void *pUserData);

    // GLFW framebuffer resize callback function.
    //
    // parameters are conforming to the callback and are not optionally removable.
    //
    // @param window GLFW window that was resized.
    // @param width framebuffer width.
    // @param height framebuffer height.
    void glfwFramebufferResizeCallback(GLFWwindow *window, int width, int height);

    // GLFW mouse button callback.
    //
    // @param window GLFW window that the mouse button action occurred in.
    // @param button the mouse button that was pressed.
    // @param action the action of the mouse(press, depress).
    // @param ?
    void glfwMouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
};


#endif  // CALLBACKS_H
