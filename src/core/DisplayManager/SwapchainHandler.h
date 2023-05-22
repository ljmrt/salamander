#ifndef SWAPCHAINHANDLER_H
#define SWAPCHAINHANDLER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/DisplayManager/DisplayManager.h>
#include <core/VulkanInstance/DeviceHandler.h>

#include <vector>


namespace SwapchainHandler
{
    struct SwapchainSupportDetails {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        std::vector<VkSurfaceFormatKHR> supportedSurfaceFormats;
        std::vector<VkPresentModeKHR> supportedPresentationModes;
    };


    // get the supported swapchain details.
    //
    // @param physicalDevice physical device to query swap chain support details of.
    // @param windowSurface window surface to use in details query.
    // @param queriedSwapchainSupportDetails stored queried swapchain support details.
    void querySwapchainSupportDetails(VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface, SwapchainSupportDetails& queriedSwapchainSupportDetails);

    // select a swapchain surface format out of a list of availible formats.
    //
    // @param availibleSurfaceFormats the availible surface formats to select from.
    // @param selectedSurfaceFormat the selected/result surface format.
    void selectSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> availibleSurfaceFormats, VkSurfaceFormatKHR& selectedSurfaceFormat);

    // select a swapchain presentation mode out of a list of availible presentation modes.
    //
    // @param availiblePresentationModes the availible presentation modes to select from.
    // @param selectedPresentationMode the selected/result presentation mode.
    void selectSwapchainPresentationMode(const std::vector<VkPresentModeKHR> availiblePresentationModes, VkPresentModeKHR& selectedPresentationMode);

    // select a swapchain image extent based on the surface capabilities.
    //
    // @param extentCapabilities the surface capabilities.
    // @param glfwWindow GLFW window to use in selection.
    // @param selectedImageExtent the selected/result swapchain image exent.
    void selectSwapchainImageExtent(const VkSurfaceCapabilitiesKHR extentCapabilities, GLFWwindow *glfwWindow, VkExtent2D& selectedImageExtent);

    // create a swapchain using the supplied parameters.
    //
    // @param physicalDevice physical device to use in swap chain creation.
    // @param logicalDevice logical device to use in swap chain creation.
    // @param glfwWindow GLFW window to use in swap chain creation.
    // @param windowSurface window surface to use in swap chain creation.
    // @param createdSwapchain stored created swap chain.
    // @param createdSwapchainImages stored created swap chain images.
    // @param createdSwapchainImageFormat stored swap chain image format.
    // @param createdSwapchainExtent stored swap chain extent.
    void createSwapchainComponents(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, GLFWwindow *glfwWindow, VkSurfaceKHR windowSurface, VkSwapchainKHR& createdSwapchain, std::vector<VkImage>& createdSwapchainImages, VkFormat& createdSwapchainImageFormat, VkExtent2D& createdSwapchainExtent);

    // create a swapchain using a display details's members.
    //
    // wrapper around createSwapchainComponents for ease-of-use.
    //
    // @param vulkanDevices Vulkan logical and physical devices.
    // @param displayDetails display details to create with and store swapchain components.
    void createSwapchainComponentsWrapper(DeviceHandler::VulkanDevices vulkanDevices, DisplayManager::DisplayDetails& displayDetails);

    // create swapchain image views.
    //
    // @param swapchainImages swapchain images to use in image view creation.
    // @param swapchainImageFormat swapchain image format to use in image view creation.
    // @param vulkanLogicalDevice the Vulkan instance's logical device.
    // @param createdSwapchainImageViews stored created swapchain image views.
    void createSwapchainImageViews(std::vector<VkImage> swapchainImages, VkFormat swapchainImageFormat, VkDevice vulkanLogicalDevice, std::vector<VkImageView>& createdSwapchainImageViews);

    // create the framebuffers necessary for all swapchain image views.
    //
    // @param swapchainImageViews swapchain image views to use in framebuffer creation.
    // @param colorImageView the color image view to use in framebuffer creation.
    // @param depthImageView the depth image view to use in framebuffer creation.
    // @param renderPass render pass to use in framebuffer creation.
    // @param swapchainExtent swapchain image extent to use in framebuffer creation.
    // @param vulkanLogicalDevice this Vulkan instance's logical device.
    // @param createdSwapchainFramebuffers stored created swapchain framebuffers.
    void createSwapchainFramebuffers(std::vector<VkImageView> swapchainImageViews, VkImageView colorImageView, VkImageView depthImageView, VkRenderPass renderPass, VkExtent2D swapchainImageExtent, VkDevice vulkanLogicalDevice, std::vector<VkFramebuffer>& createdSwapchainFramebuffers);

    // recreate the swapchain after some form of incompatibility.
    //
    // uses swapchain creation wrapper.
    //
    // @param vulkanDevices Vulkan logical and physical devices.
    // @param renderPass render pass to use in swapchain recreation.
    // @param commandPool command pool to use in swapchain recreation.
    // @param commandQueue queue to sumbit necessary commands on.
    // @param displayDetails display details to use and store recreated components in.
    void recreateSwapchain(DeviceHandler::VulkanDevices vulkanDevices, VkRenderPass renderPass, VkCommandPool commandPool, VkQueue commandQueue, DisplayManager::DisplayDetails& displayDetails);

    // clean up a swapchain.
    //
    // @param vulkanDisplayDetails vulkan display details to use in swapchain cleanup.
    // @param vulkanLogicalDevice the Vulkan instance's logical device.
    void cleanupSwapchain(DisplayManager::VulkanDisplayDetails vulkanDisplayDetails, VkDevice vulkanLogicalDevice);
}


#endif  // SWAPCHAINHANDLER_H
