#ifndef SWAPCHAINHANDLER_H
#define SWAPCHAINHANDLER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>


namespace swapchainHandler
{
    struct SwapchainSupportDetails {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        std::vector<VkSurfaceFormatKHR> supportedSurfaceFormats;
        std::vector<VkPresentModeKHR> supportedPresentationModes;
    };


    // get the supported swap chain details.
    //
    // @param physicalDevice physical device to query swap chain support details of.
    // @param windowSurface window surface to use in details query.
    // @param resultSwapchainSupportDetails stored resulting swap nchain support details.
    void querySwapchainSupportDetails(VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface, SwapchainSupportDetails& resultSwapchainSupportDetails);

    // select a swap chain surface format out of a list of availible formats.
    //
    // @param availibleSurfaceFormats the availible surface formats to select from.
    // @param selectedSurfaceFormat the selected/result surface format.
    void selectSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> availibleSurfaceFormats, VkSurfaceFormatKHR& selectedSurfaceFormat);

    // select a swap chain presentation mode out of a list of availible presentation modes.
    //
    // @param availiblePresentationModes the availible presentation modes to select from.
    // @param selectedPresentationMode the selected/result presentation mode.
    void selectSwapchainPresentationMode(const std::vector<VkPresentModeKHR> availiblePresentationModes, VkPresentModeKHR& selectedPresentationMode);

    // select a swap chain extent based on the surface capabilities.
    //
    // @param extentCapabilities the surface capabilities.
    // @param glfwWindow GLFW window to use in selection.
    // @param selectedExtent the selected/result exent.
    void selectSwapchainExtent(const VkSurfaceCapabilitiesKHR extentCapabilities, GLFWwindow *glfwWindow, VkExtent2D& selectedExtent);

    // create a swap chain using the supplied parameters.
    //
    // @param physicalDevice physical device to use in swap chain creation.
    // @param logicalDevice logical device to use in swap chain creation.
    // @param glfwWindow GLFW window to use in swap chain creation.
    // @param windowSurface window surface to use in swap chain creation.
    // @param resultSwapchain stored created swap chain.
    // @param resultSwapchainImages stored created swap chain images.
    // @param resultSwapchainImageFormat stored swap chain image format.
    // @param resultSwapchainExtent stored swap chain extent.
    void createSwapchain(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, GLFWwindow *glfwWindow, VkSurfaceKHR windowSurface, VkSwapchainKHR& resultSwapchain, std::vector<VkImage> resultSwapchainImages, VkFormat& resultSwapchainImageFormat, VkExtent2D& resultSwapchainExtent);
}


#endif  // SWAPCHAINHANDLER_H
