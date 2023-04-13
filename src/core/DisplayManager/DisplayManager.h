#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Config/Config.h>

#include <string>
#include <vector>


namespace DisplayManager
{
    struct VulkanDisplayDetails {
        VkSurfaceKHR windowSurface;

        VkSwapchainKHR swapchain;
        std::vector<VkImage> swapchainImages;
        VkFormat swapchainImageFormat;
        VkExtent2D swapchainExtent;
        std::vector<VkImageView> swapchainImageViews;
    };
    
    struct DisplayDetails {
        GLFWwindow *glfwWindow;
        VulkanDisplayDetails vulkanDisplayDetails;
    };
    
    
    // initialize and setup GLFW.
    void initializeGLFW();
    
    // create a window using the specified parameters.
    //
    // @param windowWidth width of the created window(px).
    // @param windowHeight height of the created window(px).
    // @param windowName name of the created window.
    // @param resultWindow stored created window.
    void createWindow(uint32_t windowWidth, uint32_t windowHeight, std::string windowName, GLFWwindow*& resultWindow);
    
    // create a window surface to present using.
    //
    // @param vkInstance Vulkan instance to use in surface creation.
    // @param glfwWindow GLFW window to create a surface from.
    // @param resultWindowSurface stored created window surface.
    void createWindowSurface(VkInstance vkInstance, GLFWwindow *glfwWindow, VkSurfaceKHR& resultWindowSurface);

    // create Vulkan image views.
    //
    // @param vulkanDisplayDetails Vulkan display details struct to use and store result in.
    // @param logicalDevice logical device to use in image view creation.
    void createImageViews(VulkanDisplayDetails& vulkanDisplayDetails, VkDevice logicalDevice);

    // process window input and act accordingly.
    //
    // @param glfwWindow GLFW window to take and process input from.
    void processWindowInput(GLFWwindow *glfwWindow);

    // stall window open(temporary function).
    //
    // @param glfwWindow GLFW window to stall.
    void stallWindow(GLFWwindow *glfwWindow);

    // cleanup and terminate GLFW.
    //
    // @param glfwWindow GLFW window to destroy.
    void cleanupGLFW(GLFWwindow *glfwWindow);
}


#endif  // DISPLAYMANAGER_H
