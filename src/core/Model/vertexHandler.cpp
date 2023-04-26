#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Model/vertexHandler.h>
#include <core/VulkanInstance/deviceHandler.h>
#include <core/Logging/ErrorLogger.h>

#include <glm/glm.hpp>

#include <vector>


static void vertexHandler::Vertex::fetchBindingDescription(VkVertexInputBindingDescription& bindingDescription)
{
    bindingDescription.binding = 0;  // binding index in the "array" of bindings.
    bindingDescription.stride = sizeof(vertexHandler::Vertex);  // byte distance from one entry to another.
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}

static void vertexHandler::Vertex::fetchAttributeDescriptions(std::vector<VkVertexInputAttributeDescription, 2>& attributeDescriptions)
{
    // position description.
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;  // "location" keyword in shader.
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(vertexHandler::Vertex, position);

    // color description.
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;  // "location" keyword in shader.
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(vertexHandler::Vertex, color);
}

void vertexHandler::createVertexBufferComponents(deviceHandler::VulkanDevices vulkanDevices, VkBuffer& vertexBuffer)
{
    VkBufferCreateInfo vertexBufferCreateInfo{};
    vertexBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    
    vertexBufferCreateInfo.size = sizeof(vertices[0]) * vertices.size();
    vertexBufferCreateInfo.VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vertexBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;  // only the graphics queue is using this.

    VkResult vertexBufferCreationResult = vkCreateBuffer(vulkanLogicalDevice, &vertexBufferCreateInfo, nullptr, &vertexBuffer);
    if (vertexBufferCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create vertex buffer.");
    }


    vkGetBufferMemoryRequirements(vulkanLogicalDevice, vertexBuffer, &m_memoryRequirements);
    VkDeviceMemory vertexBufferMemory;
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
    
    size_t memoryTypeIndex;
    if (findBufferMemoryType(vulkanDevices.physicalDevice, m_memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, memoryTypeIndex) == false) {
        throwDebugException("Failed to find a suitable memory type.");
    }
    memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

    VkResult memoryAllocationResult = vkAllocateMemory(vulkanDevices, &memoryAllocateInfo, nullptr, &vertexBufferMemory);
    if (memoryAllocationResult != VK_SUCCESS) {
        throwDebugException("Failed to allocate vertex buffer memory.");
    }
}
