#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Queue/Queue.h>

#include <set>
#include <vector>


void Queue::getSupportedQueueFamilies(VkPhysicalDevice device, std::vector<VkQueueFamilyProperties>& resultQueueFamilies)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    resultQueueFamilies = std::vector<VkQueueFamilyProperties>(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, resultQueueFamilies.data());
}

bool Queue::deviceQueueFamiliesSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface, QueueFamilyIndices& resultFamilyIndices)
{
    std::vector<VkQueueFamilyProperties> queueFamilies;
    Queue::getSupportedQueueFamilies(physicalDevice, queueFamilies);

    int i = 0;
    for (VkQueueFamilyProperties queueFamily : queueFamilies) {
        if (resultFamilyIndices.isAssigned()) {
            break;
        }
        
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {  // if this queue family supports graphics operations.
            resultFamilyIndices.graphicsFamily = i;
        }

        // if this queue family supports presentation operations.
        VkBool32 presentationSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, windowSurface, &presentationSupport);
        if (presentationSupport) {
            resultFamilyIndices.presentationFamily = i;
        }
        
        i += 1;
    }
    
    return resultFamilyIndices.isAssigned();  // check whether the entire struct is assigned rather than comparing families to 0 as family location could be 0.
}

void Queue::getFamilyCreateInfos(QueueFamilyIndices familyIndices, std::vector<VkDeviceQueueCreateInfo>& resultFamilyCreateInfos)
{
    // create queue families specified in QueueFamilyIndices.
    std::set<uint32_t> uniqueQueueFamilies = {familyIndices.graphicsFamily.value(), familyIndices.presentationFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo familyCreateInfo{};
        familyCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        familyCreateInfo.queueFamilyIndex = queueFamily;
        familyCreateInfo.queueCount = 1;
        familyCreateInfo.pQueuePriorities = &queuePriority;

        resultFamilyCreateInfos.push_back(familyCreateInfo);
    }
}
