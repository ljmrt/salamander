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

void vertexHandler::createVertexBufferComponents(deviceHandler::VulkanDevices vulkanDevices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory)
{
    VkBufferCreateInfo vertexBufferCreateInfo{};
    vertexBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    
    vertexBufferCreateInfo.size = sizeof(vertices[0]) * vertices.size();
    vertexBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vertexBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;  // only the graphics queue is using this.

    VkResult vertexBufferCreationResult = vkCreateBuffer(vulkanDevices.logicalDevice, &vertexBufferCreateInfo, nullptr, &vertexBuffer);
    if (vertexBufferCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create vertex buffer.");
    }


    vkGetBufferMemoryRequirements(vulkanDevices.logicalDevice, vertexBuffer, &m_memoryRequirements);
    allocateVertexBufferMemory(vulkanDevices, vertexBufferMemory);
    vkBindBufferMemory(vulkanDevices.logicalDevice, vertexBuffer, vertexBufferMemory, 0);

    
    // map vertex buffer memory and copy vertices data into it.
    void* verticesData;
    vkMapMemory(vulkanDevices.logicalDevice, vertexBufferMemory, 0, vertexBufferCreateInfo.size, 0, &verticesData);
    memcpy(verticesData, vertices.data(), (size_t)(vertexBufferCreateInfo.size));
    vkUnmapMemory(vulkanDevices.logicalDevice, vertexBufferMemory);
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

void vertexHandler::allocateVertexBufferMemory(deviceHandler::VulkanDevices vulkanDevices, VkDeviceMemory& vertexBufferMemory)
{
    VkMemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    memoryAllocateInfo.allocationSize = m_memoryRequirements.size;
    
    uint32_t memoryTypeIndex;  // prefer size_t, but better to cohere.
    if (findBufferMemoryType(vulkanDevices.physicalDevice, m_memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, memoryTypeIndex) == false) {
        throwDebugException("Failed to find a suitable memory type.");
    }
    memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

    VkResult memoryAllocationResult = vkAllocateMemory(vulkanDevices.logicalDevice, &memoryAllocateInfo, nullptr, &vertexBufferMemory);
    if (memoryAllocationResult != VK_SUCCESS) {
        throwDebugException("Failed to allocate vertex buffer memory.");
    }
}
