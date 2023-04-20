#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Queue/Queue.h>

#include <set>
#include <vector>


void Queue::querySupportedQueueFamilies(VkPhysicalDevice physicalDevice, std::vector<VkQueueFamilyProperties>& queriedQueueFamilies)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    queriedQueueFamilies = std::vector<VkQueueFamilyProperties>(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queriedQueueFamilies.data());
}

bool Queue::deviceQueueFamiliesSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface, QueueFamilyIndices& supportedFamilyIndices)
{
    std::vector<VkQueueFamilyProperties> queueFamilies;
    querySupportedQueueFamilies(physicalDevice, queueFamilies);

    int i = 0;
    for (VkQueueFamilyProperties queueFamily : queueFamilies) {
        if (supportedFamilyIndices.isPopulated()) {
            break;
        }
        
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {  // if this queue family supports graphics operations.
            supportedFamilyIndices.graphicsFamily = i;
        }

        // if this queue family supports presentation operations.
        VkBool32 presentationSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, windowSurface, &presentationSupport);
        if (presentationSupport) {
            supportedFamilyIndices.presentationFamily = i;
        }
        
        i += 1;
    }
    
    return supportedFamilyIndices.isPopulated();  // check whether the entire struct is populated rather than comparing families to 0 as family location could be 0.
}

void Queue::populateQueueCreateInfos(QueueFamilyIndices queueFamilyIndices, std::vector<VkDeviceQueueCreateInfo>& populatedQueueCreateInfos)
{
    // create queue families specified in QueueFamilyIndices.
    std::set<uint32_t> uniqueQueueFamilies = {queueFamilyIndices.graphicsFamily.value(), queueFamilyIndices.presentationFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo familyCreateInfo{};
        familyCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        familyCreateInfo.queueFamilyIndex = queueFamily;
        familyCreateInfo.queueCount = 1;
        familyCreateInfo.pQueuePriorities = &queuePriority;

        populatedQueueCreateInfos.push_back(familyCreateInfo);
    }
}
