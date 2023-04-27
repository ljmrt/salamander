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
    };

    const std::vector<Vertex> vertices = {
        // position(vec2), color(vec3).
        {{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };

    extern VkMemoryRequirements m_memoryRequirements;  // populated in vertex buffer creation.


    // TODO: refactor function parameters with prefixes(ex: fetchedXXX, createdXXX).
    
    // TODO: refactor function name.
    // fetch the binding description used for vertices.
    //
    // @param bindingDescription stored fetched binding description.
    void fetchBindingDescription(VkVertexInputBindingDescription& bindingDescription);

    // TODO: refactor function name.
    // fetch tte attribute descriptions used for the vertices.
    //
    // @param attributeDescriptions stored fetched attribute descriptions.
    void fetchAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& attributeDescriptions);

    // create vertex buffer components.
    //
    // @param vulkanDevices Vulkan devices to use in vertex buffer component creation.
    // @param vertexBuffer stored created vertex buffer.
    // @param vertexBufferMemory stored allocated vertex buffer memory.
    void createVertexBufferComponents(deviceHandler::VulkanDevices vulkanDevices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory);

    // find a memory type comformant to memory type filter and required property flags.
    //
    // used mainly in vertex buffer memory allocation.
    //
    // @param vulkanPhysicalDevice Vulkan physical device to use in selection.
    // @param memoryTypeFilter memory type bitmask/filter.
    // @param requiredMemoryPropertyFlags required for the memory property to have.
    // @param memoryType stored selected memory type.
    bool findBufferMemoryType(VkPhysicalDevice vulkanPhysicalDevice, uint32_t memoryTypeFilter, VkMemoryPropertyFlags requiredMemoryPropertyFlags, uint32_t& memoryType);

    // allocate memory for an vertex buffer.
    //
    // @param vulkanDevices Vulkan devices to use in allocation.
    // @param vertexBufferMemory stored allocated vertex buffer memory.
    void allocateVertexBufferMemory(deviceHandler::VulkanDevices vulkanDevices, VkDeviceMemory& vertexBufferMemory);
}


#endif  // VERTEXHANDLER_H
