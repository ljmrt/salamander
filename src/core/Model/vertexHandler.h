#ifndef VERTEXHANDLER_H
#define VERTEXHANDLER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/deviceHandler.h>
#include <glm/glm.hpp>

#include <vector>


namespace vertexHandler
{
    struct Vertex {
        glm::vec2 position;
        glm::vec3 color;

        static void fetchBindingDescription(VkVertexInputBindingDescription& bindingDescription);
        static void fetchAttributeDescriptions(std::vector<VkVertexInputAttributeDescription, 2>& attributeDescriptions);
    };

    const std::vector<Vertex> vertices = {
        // position(vec2), color(vec3).
        {{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };

    VkMemoryRequirements m_memoryRequirements;  // populated in vertex buffer creation.


    void createVertexBufferComponents(deviceHandler::VulkanDevices vulkanDevices, VkBuffer& vertexBuffer);

    bool findBufferMemoryType(VkPhysicalDevice vulkanPhysicalDevice, uint32_t memoryTypeFilter, VkMemoryPropertyFlags requiredMemoryPropertyFlags, uint32_t& memoryType);
    
    void allocateVertexBufferMemory(deviceHandler::VulkanDevices vulkanDevices, VkDeviceMemory& vertexBufferMemory);
}


#endif  // VERTEXHANDLER_H
