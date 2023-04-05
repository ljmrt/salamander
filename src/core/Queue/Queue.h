#ifndef QUEUE_H
#define QUEUE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <optional>
#include <vector>


namespace Queue
{
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentationFamily;

        bool isAssigned() {
            return graphicsFamily.has_value() && presentationFamily.has_value();
        }
    };
    
    
    // get the supported queue families from a physical device.
    //
    // @param device device to find the queue families of.
    // @return struct with indices of the supported queue families.
    void getSupportedQueueFamilies(VkPhysicalDevice device, std::vector<VkQueueFamilyProperties>& resultQueueFamilies);

    // TODO: refactor this function for ease-of-use(extract QueueFamilyIndices setting).
    // test whether the device supports all required queue families(specified in QueueFamilyIndices).
    //
    // @param physicalDevice physical device to check support of.
    // @param windowSurface window surface to use in presentation support testing.
    // @param resultFamilyIndices stored family indices(if supported).
    bool deviceQueueFamiliesSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface, QueueFamilyIndices& resultFamilyIndices);

    // get the VkDeviceQueueCreateInfos from the supplied indices.
    //
    // @param familyIndices indices to base the create infos on.
    // @param resultFamilyCreateInfos stored resulting VkDeviceQueueCreateInfos.
    void getFamilyCreateInfos(QueueFamilyIndices familyIndices, std::vector<VkDeviceQueueCreateInfo>& resultFamilyCreateInfos);
}


#endif  // QUEUE_H
