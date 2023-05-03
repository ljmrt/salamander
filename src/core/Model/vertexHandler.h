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

    const std::vector<Vertex> vertices = {  // represents the vertex buffer's data.
        // position(vec2), color(vec3).
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };

    const std::vector<uint16_t> indices = {  // represents the index buffer.
        0, 1, 2, 2, 3, 0
    };

    struct UniformBufferObject {  // identical to the vertex shader's struct.
        alignas(16) glm::mat4 modelMatrix;
        alignas(16) glm::mat4 viewMatrix;
        alignas(16) glm::mat4 projectionMatrix;
    };

    extern VkMemoryRequirements m_memoryRequirements;  // populated in vertex buffer creation.


    // TODO: refactor function parameters with prefixes(ex: fetchedXXX, createdXXX).
    
    // TODO: refactor function name.
    // fetch the binding description used for vertices.
    //
    // @param bindingDescription stored fetched binding description.
    void fetchBindingDescription(VkVertexInputBindingDescription& bindingDescription);

    // TODO: refactor function name.
    // fetch the attribute descriptions used for the vertices.
    //
    // @param attributeDescriptions stored fetched attribute descriptions.
    void fetchAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& attributeDescriptions);

    // create generic Vulkan buffer's components(VkBuffer, VkDeviceMemory).
    //
    // @param bufferSize the total size of the buffer/buffer memory in bytes.
    // @param memoryProperties required memory properties for the buffer's memory.
    // @param vulkanDevices Vulkan physical and logical device.
    // @param buffer created buffer.
    // @param bufferMemory allocated buffer memory.
    void createBufferComponents(VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryProperties, deviceHandler::VulkanDevices vulkanDevices, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

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
    void createDataBufferComponents(const void *bufferData, VkDeviceSize buffersSize, VkBufferUsageFlagBits bufferUsage, VkCommandPool commandPool, VkQueue transferQueue, deviceHandler::VulkanDevices vulkanDevices, VkBuffer& dataBuffer, VkDeviceMemory& dataBufferMemory);
    
    // find a memory type comformant to memory type filter and required property flags.
    //
    // used mainly in vertex buffer memory allocation.
    //
    // @param vulkanPhysicalDevice Vulkan physical device to use in selection.
    // @param memoryTypeFilter memory type bitmask/filter.
    // @param requiredMemoryPropertyFlags required for the memory property to have.
    // @param memoryType stored selected memory type.
    bool findBufferMemoryType(VkPhysicalDevice vulkanPhysicalDevice, uint32_t memoryTypeFilter, VkMemoryPropertyFlags requiredMemoryPropertyFlags, uint32_t& memoryType);

    // allocate a Vulkan buffer's memory.
    //
    // @param memoryRequirements the memory requirements of the buffer's memory.
    // @param memoryProperties the required memory properties of the buffer's memory.
    // @param vulkanDevices Vulkan physical and logical device.
    // @param bufferMemory allocated buffer memory.
    void allocateBufferMemory(VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags memoryProperties, deviceHandler::VulkanDevices vulkanDevices, VkDeviceMemory& bufferMemory);

    // copy a source buffer's memory into a destination buffer.
    //
    // @param sourceBuffer the source/supplier buffer.
    // @param destinationBuffer the destination/receiver buffer.
    // @param buffersSize the size of both buffers in bytes.
    // @param commandPool command pool to use in buffer operations.
    // @param transferQueue queue to use in buffer memory operations.
    // @param vulkanLogicalDevice Vulkan logical device.
    void copyBuffer(VkBuffer& sourceBuffer, VkBuffer& destinationBuffer, VkDeviceSize buffersSize, VkCommandPool commandPool, VkQueue transferQueue, VkDevice vulkanLogicalDevice);

    // TODO: move these to seperate(Shader directory?) file.
    // create multiple uniform buffers(one for each frame in flight).
    //
    // @param vulkanDevices Vulkan physical and logical device.
    // @param uniformBuffers created uniform buffers.
    // @param uniformBuffersMemory allocated uniform buffers memory.
    // @param mappedUniformBuffersMemory mapped uniform buffers memory.
    void createUniformBuffers(deviceHandler::VulkanDevices vulkanDevices, std::vector<VkBuffer>& uniformBuffers, std::vector<VkDeviceMemory>& uniformBuffersMemory, std::vector<void *>& mappedUniformBuffersMemory);

    // update the current frame's vertex shader uniform buffer.
    //
    // @param currentImage current image/frame.
    // @param swapchainImageExtent Vulkan swapchain image extent.
    // @param mappedUniformBuffersMemory mapped uniform buffers memory.
    void updateUniformBuffer(size_t currentImage, VkExtent2D swapchainImageExtent, std::vector<void *>& mappedUniformBuffersMemory);
}


#endif  // VERTEXHANDLER_H
