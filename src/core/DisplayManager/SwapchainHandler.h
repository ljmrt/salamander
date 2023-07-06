#ifndef SWAPCHAINHANDLER_H
#define SWAPCHAINHANDLER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/DisplayManager/DisplayManager.h>
#include <core/Renderer/Renderer.h>
#include <core/Renderer/Offscreen.h>
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

    // populate(not create) a framebuffer create info struct.
    //
    // @param renderPass the render pass to use in the framebuffer create info.
    // @param framebufferAttachments array of framebuffer attachments.
    // @param framebufferAttachmentsCount the amount of framebuffer attachments.
    // @param framebufferWidth the specified width of the framebuffer.
    // @param framebufferHeight the specified height of the framebuffer.
    // @param framebufferCreateInfo populated framebuffer create info.
    void populateFramebufferCreateInfo(VkRenderPass renderPass, VkImageView framebufferAttachments[], uint32_t framebufferAttachmentsCount, uint32_t framebufferWidth, uint32_t framebufferHeight, VkFramebufferCreateInfo& framebufferCreateInfo);
    
    // create the framebuffers necessary for all swapchain image views.
    //
    // @param swapchainImageViews swapchain image views to use in framebuffer creation.
    // @param swapchainExtent swapchain image extent to use in framebuffer creation.
    // @param colorImageView the color image view to use in framebuffer creation.
    // @param depthImageView the depth image view to use in framebuffer creation.
    // @param renderPass render pass to use in framebuffer creation.
    // @param vulkanLogicalDevice this Vulkan instance's logical device.
    // @param createdSwapchainFramebuffers stored created swapchain framebuffers.
    void createSwapchainFramebuffers(std::vector<VkImageView> swapchainImageViews, VkExtent2D swapchainImageExtent, VkImageView colorImageView, VkImageView depthImageView, VkRenderPass renderPass, VkDevice vulkanLogicalDevice, std::vector<VkFramebuffer>& createdSwapchainFramebuffers);

    // recreate the swapchain after some form of incompatibility.
    //
    // uses swapchain creation wrapper.
    //
    // @param vulkanDevices Vulkan logical and physical devices.
    // @param renderPass render pass to use in swapchain recreation.
    // @param directionalShadowOperation the directional shadow offscreen operation.
    // @param pointShadowOperation the point shadow offscreen operation.
    // @param displayDetails display details to use and store recreated components in.
    void recreateSwapchain(DeviceHandler::VulkanDevices vulkanDevices, VkRenderPass renderPass, Offscreen::OffscreenOperation& directionalShadowOperation, Offscreen::OffscreenOperation& pointShadowOperation, DisplayManager::DisplayDetails& displayDetails);

    // clean up a swapchain.
    //
    // @param displayDetails display details to use in swapchain cleanup.
    // @param vulkanLogicalDevice the Vulkan instance's logical device.
    void cleanupSwapchain(DisplayManager::DisplayDetails displayDetails, VkDevice vulkanLogicalDevice);
}


#endif  // SWAPCHAINHANDLER_H
