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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    resultWindow = glfwCreateWindow(windowWidth, windowHeight, windowName.c_str(), nullptr, nullptr);
}

void DisplayManager::createWindowSurface(VkInstance vkInstance, GLFWwindow *glfwWindow, VkSurfaceKHR& resultWindowSurface)
{
    int creationResult = glfwCreateWindowSurface(vkInstance, glfwWindow, nullptr, &resultWindowSurface);
    if (creationResult != VK_SUCCESS) {
        throwDebugException("Failed to create window surface");
    }
}

void DisplayManager::createImageViews(DisplayManager::VulkanDisplayDetails& vulkanDisplayDetails, VkDevice logicalDevice)
{
    vulkanDisplayDetails.swapchainImageViews.resize(vulkanDisplayDetails.swapchainImages.size());
    for (size_t i = 0; i < vulkanDisplayDetails.swapchainImages.size(); i += 1) {
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = vulkanDisplayDetails.swapchainImages[i];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = vulkanDisplayDetails.swapchainImageFormat;
        // swizzle/modify color channels, currently set to default mapping.
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;  // use image as color target.
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        int imageViewCreationResult = vkCreateImageView(logicalDevice, &imageViewCreateInfo, nullptr, &vulkanDisplayDetails.swapchainImageViews[i]);
        if (imageViewCreationResult != VK_SUCCESS) {
            throwDebugException("Failed to create image views.");
        }
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
