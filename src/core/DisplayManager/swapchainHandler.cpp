#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/DisplayManager/swapchainHandler.h>
#include <core/DisplayManager/DisplayManager.h>
#include <core/VulkanInstance/deviceHandler.h>
#include <core/Queue/Queue.h>
#include <core/Logging/ErrorLogger.h>

#include <limits>
#include <algorithm>


void swapchainHandler::querySwapchainSupportDetails(VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface, SwapchainSupportDetails& queriedSwapchainSupportDetails)
{
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, windowSurface, &queriedSwapchainSupportDetails.surfaceCapabilities);

    uint32_t supportedFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, windowSurface, &supportedFormatCount, nullptr);
    if (supportedFormatCount != 0) {
        queriedSwapchainSupportDetails.supportedSurfaceFormats.resize(supportedFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, windowSurface, &supportedFormatCount, queriedSwapchainSupportDetails.supportedSurfaceFormats.data());
    }

    uint32_t supportedPresentationModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, windowSurface, &supportedPresentationModeCount, nullptr);
    if (supportedFormatCount != 0) {
        queriedSwapchainSupportDetails.supportedPresentationModes.resize(supportedPresentationModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, windowSurface, &supportedPresentationModeCount, queriedSwapchainSupportDetails.supportedPresentationModes.data());
    }
}

void swapchainHandler::selectSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> availibleSurfaceFormats, VkSurfaceFormatKHR& selectedSurfaceFormat)
{
    for (VkSurfaceFormatKHR surfaceFormat : availibleSurfaceFormats) {
        if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            selectedSurfaceFormat = surfaceFormat;
        }
    }
    
    selectedSurfaceFormat = availibleSurfaceFormats[0];  // select first format in the availible surface formats as a fallback.
}

void swapchainHandler::selectSwapchainPresentationMode(const std::vector<VkPresentModeKHR> availiblePresentationModes, VkPresentModeKHR& selectedPresentationMode)
{
    for (VkPresentModeKHR presentationMode : availiblePresentationModes) {
        if (presentationMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            selectedPresentationMode = presentationMode;
        }
    }

    selectedPresentationMode = VK_PRESENT_MODE_FIFO_KHR;  // a less preferred but guarenteed to be availible presentation mode.
}

void swapchainHandler::selectSwapchainImageExtent(const VkSurfaceCapabilitiesKHR extentCapabilities, GLFWwindow *glfwWindow, VkExtent2D& selectedImageExtent)
{
    if (extentCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        selectedImageExtent = extentCapabilities.currentExtent;  // swap chain extent is not flexible.
    } else {
        int framebufferPixelWidth;
        int framebufferPixelHeight;
        glfwGetFramebufferSize(glfwWindow, &framebufferPixelWidth, &framebufferPixelHeight);

        selectedImageExtent = {
            static_cast<uint32_t>(framebufferPixelWidth),
            static_cast<uint32_t>(framebufferPixelHeight)
        };
        selectedImageExtent.width = std::clamp(selectedImageExtent.width, extentCapabilities.minImageExtent.width, extentCapabilities.maxImageExtent.width);
        selectedImageExtent.height = std::clamp(selectedImageExtent.height, extentCapabilities.minImageExtent.height, extentCapabilities.maxImageExtent.height);
    }
}

void swapchainHandler::createSwapchainComponents(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, GLFWwindow *glfwWindow, VkSurfaceKHR windowSurface, VkSwapchainKHR& createdSwapchain, std::vector<VkImage>& createdSwapchainImages, VkFormat& createdSwapchainImageFormat, VkExtent2D& createdSwapchainExtent)
{
    SwapchainSupportDetails swapchainSupportDetails;
    querySwapchainSupportDetails(physicalDevice, windowSurface, swapchainSupportDetails);

    VkSurfaceFormatKHR swapchainSurfaceFormat;
    selectSwapchainSurfaceFormat(swapchainSupportDetails.supportedSurfaceFormats, swapchainSurfaceFormat);

    VkPresentModeKHR swapchainPresentationMode;
    selectSwapchainPresentationMode(swapchainSupportDetails.supportedPresentationModes, swapchainPresentationMode);

    VkExtent2D swapchainExtent;
    selectSwapchainImageExtent(swapchainSupportDetails.surfaceCapabilities, glfwWindow, swapchainExtent);

    uint32_t swapchainImageCount = swapchainSupportDetails.surfaceCapabilities.minImageCount + 1;  // add one to prevent stalling while waiting for internal operations.
    if (swapchainSupportDetails.surfaceCapabilities.maxImageCount > 0 && swapchainImageCount > swapchainSupportDetails.surfaceCapabilities.maxImageCount) {  // if there is a maximum image count and swapchainImageCount is above it.
        swapchainImageCount = swapchainSupportDetails.surfaceCapabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo{};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    
    swapchainCreateInfo.surface = windowSurface;
    
    swapchainCreateInfo.minImageCount = swapchainImageCount;
    swapchainCreateInfo.imageFormat = swapchainSurfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = swapchainSurfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = swapchainExtent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    
    Queue::QueueFamilyIndices familyIndices;
    deviceQueueFamiliesSuitable(physicalDevice, windowSurface, familyIndices);
    
    uint32_t familyIndicesExplicit[] = {familyIndices.graphicsFamily.value(), familyIndices.presentationFamily.value()};
    if (familyIndices.graphicsFamily != familyIndices.presentationFamily) {  // if the graphics and presentation families are in seperate queue families.
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;  // use image across multiple queue families without explicit transfer of ownership.
        
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = familyIndicesExplicit;
    } else {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;  // use image in one queue family at a time and explicitly transfer ownership.
        
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    }

    swapchainCreateInfo.preTransform = swapchainSupportDetails.surfaceCapabilities.currentTransform;  // no transformation.
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;  // ignore the alpha channel in window blending.
    swapchainCreateInfo.presentMode = swapchainPresentationMode;
    swapchainCreateInfo.clipped = VK_TRUE;  // do not care about the color of obscured pixels.
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    VkResult swapchainCreationResult = vkCreateSwapchainKHR(logicalDevice, &swapchainCreateInfo, nullptr, &createdSwapchain);
    if (swapchainCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create swap chain.");
    }
    

    vkGetSwapchainImagesKHR(logicalDevice, createdSwapchain, &swapchainImageCount, nullptr);
    createdSwapchainImages.resize(swapchainImageCount);
    vkGetSwapchainImagesKHR(logicalDevice, createdSwapchain, &swapchainImageCount, createdSwapchainImages.data());


    // set output function parameters.
    createdSwapchainImageFormat = swapchainSurfaceFormat.format;
    createdSwapchainExtent = swapchainExtent;
}

void swapchainHandler::createSwapchainComponentsWrapper(deviceHandler::VulkanDevices vulkanDevices, DisplayManager::DisplayDetails& displayDetails)
{
    createSwapchainComponents(vulkanDevices.physicalDevice, vulkanDevices.logicalDevice, displayDetails.glfwWindow, displayDetails.vulkanDisplayDetails.windowSurface, displayDetails.vulkanDisplayDetails.swapchain, displayDetails.vulkanDisplayDetails.swapchainImages, displayDetails.vulkanDisplayDetails.swapchainImageFormat, displayDetails.vulkanDisplayDetails.swapchainImageExtent);
}

void swapchainHandler::createSwapchainImageViews(std::vector<VkImage> swapchainImages, VkFormat swapchainImageFormat, VkDevice vulkanLogicalDevice, std::vector<VkImageView>& createdSwapchainImageViews)
{
    createdSwapchainImageViews.resize(swapchainImages.size());
    for (size_t i = 0; i < swapchainImages.size(); i += 1) {
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = swapchainImages[i];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = swapchainImageFormat;
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

        VkResult imageViewCreationResult = vkCreateImageView(vulkanLogicalDevice, &imageViewCreateInfo, nullptr, &createdSwapchainImageViews[i]);
        if (imageViewCreationResult != VK_SUCCESS) {
            throwDebugException("Failed to create image views.");
        }
    }
}

void swapchainHandler::createSwapchainFramebuffers(std::vector<VkImageView> swapchainImageViews, VkRenderPass renderPass, VkExtent2D swapchainImageExtent, VkDevice vulkanLogicalDevice, std::vector<VkFramebuffer>& createdSwapchainFramebuffers)
{
    createdSwapchainFramebuffers.resize(swapchainImageViews.size());
    for (size_t i = 0; i < swapchainImageViews.size(); i += 1) {
        VkImageView framebufferAttachments[1] = {  // framebuffer create info requires a array of attachments.
            swapchainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferCreateInfo{};
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

        framebufferCreateInfo.renderPass = renderPass;
        
        framebufferCreateInfo.attachmentCount = 1;
        framebufferCreateInfo.pAttachments = framebufferAttachments;

        // set framebuffer width and height to the same resolution as the swapchain images.
        framebufferCreateInfo.width = swapchainImageExtent.width;
        framebufferCreateInfo.height = swapchainImageExtent.height;
        
        framebufferCreateInfo.layers = 1;

        VkResult framebufferCreationResult = vkCreateFramebuffer(vulkanLogicalDevice, &framebufferCreateInfo, nullptr, &createdSwapchainFramebuffers[i]);
        if (framebufferCreationResult != VK_SUCCESS) {
            throwDebugException("Failed to create framebuffer.");
        }
    }
}

void swapchainHandler::recreateSwapchain(deviceHandler::VulkanDevices vulkanDevices, VkRenderPass renderPass, DisplayManager::DisplayDetails& displayDetails)
{
    // stall window if minimized.
    // prefer to use size_t, but complying with GLFW is better.
    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(displayDetails.glfwWindow, &framebufferWidth, &framebufferHeight);
    
    while (framebufferWidth == 0 || framebufferHeight == 0) {  // window is minimized.
        glfwGetFramebufferSize(displayDetails.glfwWindow, &framebufferWidth, &framebufferHeight);
        glfwWaitEvents();
    }
    
    
    vkDeviceWaitIdle(vulkanDevices.logicalDevice);  // wait for logical device processing to finish.

    cleanupSwapchain(vulkanDevices.logicalDevice, displayDetails.vulkanDisplayDetails.swapchainFramebuffers, displayDetails.vulkanDisplayDetails.swapchainImageViews, displayDetails.vulkanDisplayDetails.swapchain);

    createSwapchainComponentsWrapper(vulkanDevices, displayDetails);
    createSwapchainImageViews(displayDetails.vulkanDisplayDetails.swapchainImages, displayDetails.vulkanDisplayDetails.swapchainImageFormat, vulkanDevices.logicalDevice, displayDetails.vulkanDisplayDetails.swapchainImageViews);
    createSwapchainFramebuffers(displayDetails.vulkanDisplayDetails.swapchainImageViews, renderPass, displayDetails.vulkanDisplayDetails.swapchainImageExtent, vulkanDevices.logicalDevice, displayDetails.vulkanDisplayDetails.swapchainFramebuffers);
}

void swapchainHandler::cleanupSwapchain(VkDevice vulkanLogicalDevice, std::vector<VkFramebuffer> swapchainFramebuffers, std::vector<VkImageView> swapchainImageViews, VkSwapchainKHR swapchain)
{
    for (VkFramebuffer swapchainFramebuffer : swapchainFramebuffers) {
        vkDestroyFramebuffer(vulkanLogicalDevice, swapchainFramebuffer, nullptr);
    }
    
    for (VkImageView imageView : swapchainImageViews) {
        vkDestroyImageView(vulkanLogicalDevice, imageView, nullptr);
    }
    
    vkDestroySwapchainKHR(vulkanLogicalDevice, swapchain, nullptr);
}
