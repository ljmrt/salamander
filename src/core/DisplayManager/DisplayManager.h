#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Shader/Image.h>
#include <core/Config/Config.h>

#include <string>
#include <vector>


namespace DisplayManager
{
    struct VulkanDisplayDetails {
        VkQueue graphicsQueue;
        VkQueue presentationQueue;
        
        VkSurfaceKHR windowSurface;

        VkSwapchainKHR swapchain;
        std::vector<VkImage> swapchainImages;
        VkFormat swapchainImageFormat;
        VkExtent2D swapchainImageExtent;
        std::vector<VkImageView> swapchainImageViews;
        std::vector<VkFramebuffer> swapchainFramebuffers;

        VkSampleCountFlagBits msaaSampleCount = VK_SAMPLE_COUNT_1_BIT;  // overwritten by functions.
        Image::ImageDetails colorImageDetails;  // the multisampling offscreen image details.
        Image::ImageDetails depthImageDetails;
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
    void createWindow(size_t windowWidth, size_t windowHeight, std::string windowName, GLFWwindow*& resultWindow);
    
    // create a window surface to present using.
    //
    // @param vkInstance Vulkan instance to use in surface creation.
    // @param glfwWindow GLFW window to create a surface from.
    // @param resultWindowSurface stored created window surface.
    void createWindowSurface(VkInstance vkInstance, GLFWwindow *glfwWindow, VkSurfaceKHR& resultWindowSurface);

    // process window input and act accordingly.
    //
    // @param glfwWindow GLFW window to take and process input from.
    void processWindowInput(GLFWwindow *glfwWindow);

    // cleanup and terminate GLFW.
    //
    // @param glfwWindow GLFW window to destroy.
    void cleanupGLFW(GLFWwindow *glfwWindow);
}


#endif  // DISPLAYMANAGER_H
