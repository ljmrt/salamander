#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Model/vertexHandler.h>
#include <core/VulkanInstance/deviceHandler.h>
#include <core/Logging/ErrorLogger.h>

#include <glm/glm.hpp>

#include <vector>
#include <cstring>


VkMemoryRequirements vertexHandler::m_memoryRequirements;

void vertexHandler::fetchBindingDescription(VkVertexInputBindingDescription& bindingDescription)
{
    bindingDescription.binding = 0;  // binding index in the "array" of bindings.
    bindingDescription.stride = sizeof(Vertex);  // byte distance from one entry to another.
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}

void vertexHandler::fetchAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& attributeDescriptions)
{
    VkVertexInputAttributeDescription attributeDescription{};
    
    // position description.
    attributeDescription.binding = 0;
    attributeDescription.location = 0;  // "location" keyword in shader.
    attributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescription.offset = offsetof(Vertex, position);

    attributeDescriptions.push_back(attributeDescription);

    // color description.
    attributeDescription.binding = 0;
    attributeDescription.location = 1;  // "location" keyword in shader.
    attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescription.offset = offsetof(Vertex, color);

    attributeDescriptions.push_back(attributeDescription);
}

void vertexHandler::createBufferComponents(VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryProperties, deviceHandler::VulkanDevices vulkanDevices, VkBuffer& createdBuffer, VkDeviceMemory& allocatedBufferMemory)
{
    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    
    bufferCreateInfo.size = bufferSize;
    bufferCreateInfo.usage = bufferUsage;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult bufferCreationResult = vkCreateBuffer(vulkanDevices.logicalDevice, &bufferCreateInfo, nullptr, &createdBuffer);
    if (bufferCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create buffer.");
    }


    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(vulkanDevices.logicalDevice, createdBuffer, &memoryRequirements);
    
    allocateBufferMemory(memoryRequirements, memoryProperties, vulkanDevices, allocatedBufferMemory);
    vkBindBufferMemory(vulkanDevices.logicalDevice, createdBuffer, allocatedBufferMemory, 0);
}

void vertexHandler::createVertexBufferComponents(deviceHandler::VulkanDevices vulkanDevices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory, VkCommandPool commandPool, VkQueue transferQueue)
{
    VkDeviceSize buffersSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBufferComponents(buffersSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vulkanDevices, stagingBuffer, stagingBufferMemory);  // staging buffer.
    
    // map staging buffer memory and copy vertices data into it.
    void* verticesData;
    vkMapMemory(vulkanDevices.logicalDevice, stagingBufferMemory, 0, buffersSize, 0, &verticesData);
    memcpy(verticesData, vertices.data(), (size_t)(buffersSize));
    vkUnmapMemory(vulkanDevices.logicalDevice, stagingBufferMemory);

    
    createBufferComponents(buffersSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vulkanDevices, vertexBuffer, vertexBufferMemory);

    copyBuffer(stagingBuffer, vertexBuffer, buffersSize, commandPool, vulkanDevices.logicalDevice, transferQueue);

    vkDestroyBuffer(vulkanDevices.logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(vulkanDevices.logicalDevice, stagingBufferMemory, nullptr);
}

bool vertexHandler::findBufferMemoryType(VkPhysicalDevice vulkanPhysicalDevice, uint32_t memoryTypeFilter, VkMemoryPropertyFlags requiredMemoryPropertyFlags, uint32_t& memoryType)
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

void vertexHandler::allocateBufferMemory(VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags memoryProperties, deviceHandler::VulkanDevices vulkanDevices, VkDeviceMemory& bufferMemory)
{
    VkMemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    
    uint32_t memoryTypeIndex;  // prefer size_t, but better to cohere.
    if (findBufferMemoryType(vulkanDevices.physicalDevice, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, memoryTypeIndex) == false) {
        throwDebugException("Failed to find a suitable memory type.");
    }
    memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

    VkResult memoryAllocationResult = vkAllocateMemory(vulkanDevices.logicalDevice, &memoryAllocateInfo, nullptr, &bufferMemory);
    if (memoryAllocationResult != VK_SUCCESS) {
        throwDebugException("Failed to allocate buffer memory.");
    }
}

void vertexHandler::copyBuffer(VkBuffer& sourceBuffer, VkBuffer& destinationBuffer, VkDeviceSize buffersSize, VkCommandPool commandPool, VkDevice vulkanLogicalDevice, VkQueue transferQueue)
{
    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.commandBufferCount = 1;

    VkCommandBuffer disposableCommandBuffer;
    
    vkAllocateCommandBuffers(vulkanLogicalDevice, &commandBufferAllocateInfo, &disposableCommandBuffer);


    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    
    vkBeginCommandBuffer(disposableCommandBuffer, &commandBufferBeginInfo);


    VkBufferCopy bufferCopyRegion;
    
    bufferCopyRegion.srcOffset = 0;
    bufferCopyRegion.dstOffset = 0;
    bufferCopyRegion.size = buffersSize;

    vkCmdCopyBuffer(disposableCommandBuffer, sourceBuffer, destinationBuffer, 1, &bufferCopyRegion);
    vkEndCommandBuffer(disposableCommandBuffer);


    VkSubmitInfo queueSubmitInfo{};
    queueSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    queueSubmitInfo.commandBufferCount = 1;
    queueSubmitInfo.pCommandBuffers = &disposableCommandBuffer;

    vkQueueSubmit(transferQueue, 1, &queueSubmitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(transferQueue);  // could use fences for optimization.

    vkFreeCommandBuffers(vulkanLogicalDevice, commandPool, 1, &disposableCommandBuffer);
}
