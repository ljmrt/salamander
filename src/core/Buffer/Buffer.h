#ifndef BUFFER_H
#define BUFFER_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/DeviceHandler.h>
#include <glm/glm.hpp>

#include <vector>


namespace Buffer
{
    // create generic Vulkan buffer's components(VkBuffer, VkDeviceMemory).
    //
    // @param bufferSize the total size of the buffer/buffer memory in bytes.
    // @param memoryProperties required memory properties for the buffer's memory.
    // @param vulkanDevices Vulkan physical and logical device.
    // @param buffer created buffer.
    // @param bufferMemory allocated buffer memory.
    void createBufferComponents(VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryProperties, DeviceHandler::VulkanDevices vulkanDevices, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    // create data buffer(ex: vertex buffer) components.
    //
    // uses a staging buffer.
    //
    // @param bufferData pointer to start of data to be inserted in the data buffer(ex: std::vector.data()).
    // @param buffersSize the total size of the buffer/buffer data in bytes.
    // @param bufferUsage the data buffer's Vulkan usage.
    // @param commandPool command pool to use in buffer operations.
    // @param transferQueue queue to use in buffer memory operations.
    // @param vulkanDevices Vulkan physical and logical device.
    // @param dataBuffer data buffer to create/store buffer data in.
    // @param dataBufferMemory the data buffer's allocated memory.
    void createDataBufferComponents(const void *bufferData, VkDeviceSize buffersSize, VkBufferUsageFlagBits bufferUsage, VkCommandPool commandPool, VkQueue transferQueue, DeviceHandler::VulkanDevices vulkanDevices, VkBuffer& dataBuffer, VkDeviceMemory& dataBufferMemory);
    
    // locate a memory type comformant to memory type filter and required property flags.
    //
    // @param vulkanPhysicalDevice Vulkan physical device to use in selection.
    // @param memoryTypeFilter memory type bitmask/filter.
    // @param requiredMemoryPropertyFlags required memory properties for the memory to have.
    // @param memoryType located memory type.
    // @return location success.
    bool locateMemoryType(VkPhysicalDevice vulkanPhysicalDevice, uint32_t memoryTypeFilter, VkMemoryPropertyFlags requiredMemoryPropertyFlags, uint32_t& memoryType);

    // allocate a Vulkan buffer's memory.
    //
    // @param memoryRequirements the memory requirements of the buffer's memory.
    // @param memoryProperties the required memory properties of the buffer's memory.
    // @param vulkanDevices Vulkan physical and logical device.
    // @param bufferMemory allocated buffer memory.
    void allocateBufferMemory(VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags memoryProperties, DeviceHandler::VulkanDevices vulkanDevices, VkDeviceMemory& bufferMemory);

    // copy a source buffer's memory into a destination buffer.
    //
    // @param sourceBuffer the source/supplier buffer.
    // @param destinationBuffer the destination/receiver buffer.
    // @param buffersSize the size of both buffers in bytes.
    // @param commandPool command pool to use in buffer operations.
    // @param transferQueue queue to use in buffer memory operations.
    // @param vulkanLogicalDevice Vulkan logical device.
    void copyBuffer(VkBuffer& sourceBuffer, VkBuffer& destinationBuffer, VkDeviceSize buffersSize, VkCommandPool commandPool, VkQueue transferQueue, VkDevice vulkanLogicalDevice);
}


#endif  // BUFFER_H
