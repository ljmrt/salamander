#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <core/Model/vertexHandler.h>
#include <core/VulkanInstance/deviceHandler.h>
#include <core/Defaults/Defaults.h>
#include <core/Logging/ErrorLogger.h>

#include <vector>
#include <cstring>
#include <chrono>


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

void vertexHandler::createBufferComponents(VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryProperties, deviceHandler::VulkanDevices vulkanDevices, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
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

void vertexHandler::createDataBufferComponents(const void *bufferData, VkDeviceSize buffersSize, VkBufferUsageFlagBits bufferUsage, VkCommandPool commandPool, VkQueue transferQueue, deviceHandler::VulkanDevices vulkanDevices, VkBuffer& dataBuffer, VkDeviceMemory& dataBufferMemory)
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

void vertexHandler::copyBuffer(VkBuffer& sourceBuffer, VkBuffer& destinationBuffer, VkDeviceSize buffersSize, VkCommandPool commandPool, VkQueue transferQueue, VkDevice vulkanLogicalDevice)
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

void vertexHandler::createUniformBuffers(deviceHandler::VulkanDevices vulkanDevices, std::vector<VkBuffer>& uniformBuffers, std::vector<VkDeviceMemory>& uniformBuffersMemory, std::vector<void *>& mappedUniformBuffersMemory)
{
    VkDeviceSize uniformBufferObjectSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT);
    mappedUniformBuffersMemory.resize(Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT; i += 1) {
        createBufferComponents(uniformBufferObjectSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vulkanDevices, uniformBuffers[i], uniformBuffersMemory[i]);

        vkMapMemory(vulkanDevices.logicalDevice, uniformBuffersMemory[i], 0, uniformBufferObjectSize, 0, &mappedUniformBuffersMemory[i]);
    }
}

void vertexHandler::updateUniformBuffer(size_t currentImage, VkExtent2D swapchainImageExtent, std::vector<void *>& mappedUniformBuffersMemory)
{
    // necessary to doing operations regardless of frame rate.
    static std::chrono::time_point startTime = std::chrono::high_resolution_clock::now();

    std::chrono::time_point currentTime = std::chrono::high_resolution_clock::now();
    float passedTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();


    UniformBufferObject uniformBufferObject{};

    uniformBufferObject.modelMatrix = glm::rotate(glm::mat4(1.0f), passedTime * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));  // rotate around the z-axis over time.
    uniformBufferObject.viewMatrix = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));  // look down at the geometry from above at a 45-degree angle.
    uniformBufferObject.projectionMatrix = glm::perspective(glm::radians(45.0f), swapchainImageExtent.width / (float)(swapchainImageExtent.height), 0.1f, 10.0f);  // 45-degree vertical field-of-view.
    uniformBufferObject.projectionMatrix[1][1] *= -1;  // compensate for GLM's OpenGL design, invert the y-axis.


    memcpy(mappedUniformBuffersMemory[currentImage], &uniformBufferObject, sizeof(uniformBufferObject));
}
