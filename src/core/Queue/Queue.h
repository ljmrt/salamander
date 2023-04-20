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

        bool isPopulated() {
            return graphicsFamily.has_value() && presentationFamily.has_value();
        }
    };
    
    
    // query the supported queue families of a physical device.
    //
    // @param physicalDevice physical device to use in queue family fetch.
    // @param queriedQueueFamilies stored queried queue families from the physical device.
    void querySupportedQueueFamilies(VkPhysicalDevice physicalDevice, std::vector<VkQueueFamilyProperties>& queriedQueueFamilies);

    // test whether the device supports all required queue families(specified in QueueFamilyIndices).
    //
    // @param physicalDevice physical device to check support of.
    // @param windowSurface window surface to use in presentation support testing.
    // @param supportedFamilyIndices populated queue family indices(if supported).
    bool deviceQueueFamiliesSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface, QueueFamilyIndices& supportedFamilyIndices);

    // populate the VkDeviceQueueCreateInfos from the supplied indices.
    //
    // @param queueFamilyIndices queue family indices to base the queue create infos on.
    // @param populatedQueueCreateInfos populated queue create infos.
    void populateQueueCreateInfos(QueueFamilyIndices queueFamilyIndices, std::vector<VkDeviceQueueCreateInfo>& populatedQueueCreateInfos);
}


#endif  // QUEUE_H
