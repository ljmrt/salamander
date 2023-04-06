#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/DisplayManager/swapchainHandler.h>
#include <core/Queue/Queue.h>
#include <core/Logging/ErrorLogger.h>

#include <limits>
#include <algorithm>


void swapchainHandler::querySwapchainSupportDetails(VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface, SwapchainSupportDetails& resultSwapchainSupportDetails)
{
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, windowSurface, &resultSwapchainSupportDetails.surfaceCapabilities);

    uint32_t supportedFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, windowSurface, &supportedFormatCount, nullptr);
    if (supportedFormatCount != 0) {
        resultSwapchainSupportDetails.supportedSurfaceFormats.resize(supportedFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, windowSurface, &supportedFormatCount, resultSwapchainSupportDetails.supportedSurfaceFormats.data());
    }

    uint32_t supportedPresentationModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, windowSurface, &supportedPresentationModeCount, nullptr);
    if (supportedFormatCount != 0) {
        resultSwapchainSupportDetails.supportedPresentationModes.resize(supportedPresentationModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, windowSurface, &supportedPresentationModeCount, resultSwapchainSupportDetails.supportedPresentationModes.data());
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

void swapchainHandler::selectSwapchainExtent(const VkSurfaceCapabilitiesKHR extentCapabilities, GLFWwindow *glfwWindow, VkExtent2D& selectedExtent)
{
    if (extentCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        selectedExtent = extentCapabilities.currentExtent;  // swap chain extent is not flexible.
    } else {
        int framebufferPixelWidth;
        int framebufferPixelHeight;
        glfwGetFramebufferSize(glfwWindow, &framebufferPixelWidth, &framebufferPixelHeight);

        selectedExtent = {
            static_cast<uint32_t>(framebufferPixelWidth),
            static_cast<uint32_t>(framebufferPixelHeight)
        };
        selectedExtent.width = std::clamp(selectedExtent.width, extentCapabilities.minImageExtent.width, extentCapabilities.maxImageExtent.width);
        selectedExtent.height = std::clamp(selectedExtent.height, extentCapabilities.minImageExtent.height, extentCapabilities.maxImageExtent.height);
    }
}

void swapchainHandler::createSwapchain(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, GLFWwindow *glfwWindow, VkSurfaceKHR windowSurface, VkSwapchainKHR& resultSwapchain, std::vector<VkImage> resultSwapchainImages, VkFormat& resultSwapchainImageFormat, VkExtent2D& resultSwapchainExtent)
{
    SwapchainSupportDetails swapchainSupportDetails;
    swapchainHandler::querySwapchainSupportDetails(physicalDevice, windowSurface, swapchainSupportDetails);

    VkSurfaceFormatKHR swapchainSurfaceFormat;
    swapchainHandler::selectSwapchainSurfaceFormat(swapchainSupportDetails.supportedSurfaceFormats, swapchainSurfaceFormat);

    VkPresentModeKHR swapchainPresentationMode;
    swapchainHandler::selectSwapchainPresentationMode(swapchainSupportDetails.supportedPresentationModes, swapchainPresentationMode);

    VkExtent2D swapchainExtent;
    swapchainHandler::selectSwapchainExtent(swapchainSupportDetails.surfaceCapabilities, glfwWindow, swapchainExtent);

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

    int swapchainCreationResult = vkCreateSwapchainKHR(logicalDevice, &swapchainCreateInfo, nullptr, &resultSwapchain);
    if (swapchainCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create swap chain.");
    }

    vkGetSwapchainImagesKHR(logicalDevice, resultSwapchain, &swapchainImageCount, nullptr);
    resultSwapchainImages.resize(swapchainImageCount);
    vkGetSwapchainImagesKHR(logicalDevice, resultSwapchain, &swapchainImageCount, resultSwapchainImages.data());

    resultSwapchainImageFormat = swapchainSurfaceFormat.format;
    resultSwapchainExtent = swapchainExtent;
}
