#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Buffer/Buffer.h>
#include <core/VulkanInstance/DeviceHandler.h>
#include <core/Command/CommandManager.h>
#include <core/Defaults/Defaults.h>
#include <core/Logging/ErrorLogger.h>

#include <vector>
#include <cstring>


void Buffer::createBufferComponents(VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryProperties, DeviceHandler::VulkanDevices vulkanDevices, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    
    bufferCreateInfo.size = bufferSize;
    bufferCreateInfo.usage = bufferUsage;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult bufferCreationResult = vkCreateBuffer(vulkanDevices.logicalDevice, &bufferCreateInfo, nullptr, &buffer);
    if (bufferCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create buffer.");
    }


    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(vulkanDevices.logicalDevice, buffer, &memoryRequirements);
    
    allocateBufferMemory(memoryRequirements, memoryProperties, vulkanDevices, bufferMemory);
    vkBindBufferMemory(vulkanDevices.logicalDevice, buffer, bufferMemory, 0);
}

void Buffer::createDataBufferComponents(const void *bufferData, VkDeviceSize buffersSize, VkBufferUsageFlagBits bufferUsage, VkCommandPool commandPool, VkQueue transferQueue, DeviceHandler::VulkanDevices vulkanDevices, VkBuffer& dataBuffer, VkDeviceMemory& dataBufferMemory)
{
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBufferComponents(buffersSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vulkanDevices, stagingBuffer, stagingBufferMemory);  // staging buffer.


    // map staging buffer memory and copy buffer data into it.
    void* stagingBufferData;
    vkMapMemory(vulkanDevices.logicalDevice, stagingBufferMemory, 0, buffersSize, 0, &stagingBufferData);
    memcpy(stagingBufferData, bufferData, (size_t)(buffersSize));
    vkUnmapMemory(vulkanDevices.logicalDevice, stagingBufferMemory);
    

    createBufferComponents(buffersSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | bufferUsage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vulkanDevices, dataBuffer, dataBufferMemory);

    copyBuffer(stagingBuffer, dataBuffer, buffersSize, commandPool, transferQueue, vulkanDevices.logicalDevice);

    vkDestroyBuffer(vulkanDevices.logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(vulkanDevices.logicalDevice, stagingBufferMemory, nullptr);
}

bool Buffer::locateMemoryType(VkPhysicalDevice vulkanPhysicalDevice, uint32_t memoryTypeFilter, VkMemoryPropertyFlags requiredMemoryPropertyFlags, uint32_t& memoryType)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(vulkanPhysicalDevice, &memoryProperties);

    for (size_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if (memoryTypeFilter & (1 << i)
            && (memoryProperties.memoryTypes[i].propertyFlags & requiredMemoryPropertyFlags) == requiredMemoryPropertyFlags) {  // bitmask: if all memory properties of the memory type align with the required property flags.
            memoryType = i;

            return true;
        }
    }

    return false;
}

void Buffer::allocateBufferMemory(VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags memoryProperties, DeviceHandler::VulkanDevices vulkanDevices, VkDeviceMemory& bufferMemory)
{
    VkMemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    
    uint32_t memoryTypeIndex;  // prefer size_t, but better to cohere.
    if (locateMemoryType(vulkanDevices.physicalDevice, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, memoryTypeIndex) == false) {
        throwDebugException("Failed to find a suitable memory type.");
    }
    memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

    VkResult memoryAllocationResult = vkAllocateMemory(vulkanDevices.logicalDevice, &memoryAllocateInfo, nullptr, &bufferMemory);
    if (memoryAllocationResult != VK_SUCCESS) {
        throwDebugException("Failed to allocate buffer memory.");
    }
}

void Buffer::copyBuffer(VkBuffer& sourceBuffer, VkBuffer& destinationBuffer, VkDeviceSize buffersSize, VkCommandPool commandPool, VkQueue transferQueue, VkDevice vulkanLogicalDevice)
{
    VkCommandBuffer disposableCommandBuffer;
    CommandManager::beginRecordingSingleSubmitCommands(commandPool, vulkanLogicalDevice, disposableCommandBuffer);

    
    VkBufferCopy bufferCopyRegion;
    
    bufferCopyRegion.srcOffset = 0;
    bufferCopyRegion.dstOffset = 0;
    bufferCopyRegion.size = buffersSize;

    vkCmdCopyBuffer(disposableCommandBuffer, sourceBuffer, destinationBuffer, 1, &bufferCopyRegion);


    CommandManager::submitSingleSubmitCommands(disposableCommandBuffer, commandPool, transferQueue, vulkanLogicalDevice);
}
