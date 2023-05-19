#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <core/Shader/Uniform.h>
#include <core/Buffer/Buffer.h>
#include <core/Defaults/Defaults.h>

#include <vector>
#include <cstring>
#include <chrono>


void Uniform::createUniformBuffers(DeviceHandler::VulkanDevices vulkanDevices, std::vector<VkBuffer>& uniformBuffers, std::vector<VkDeviceMemory>& uniformBuffersMemory, std::vector<void *>& mappedUniformBuffersMemory)
{
    VkDeviceSize uniformBufferObjectSize = sizeof(Uniform::UniformBufferObject);

    uniformBuffers.resize(Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT);
    mappedUniformBuffersMemory.resize(Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT; i += 1) {
        Buffer::createBufferComponents(uniformBufferObjectSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vulkanDevices, uniformBuffers[i], uniformBuffersMemory[i]);

        vkMapMemory(vulkanDevices.logicalDevice, uniformBuffersMemory[i], 0, uniformBufferObjectSize, 0, &mappedUniformBuffersMemory[i]);
    }
}

void Uniform::updateFrameUniformBuffer(size_t currentImage, VkExtent2D swapchainImageExtent, std::vector<void *>& mappedUniformBuffersMemory)
{
    // necessary to doing operations regardless of frame rate.
    static std::chrono::time_point startTime = std::chrono::high_resolution_clock::now();

    std::chrono::time_point currentTime = std::chrono::high_resolution_clock::now();
    float passedTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();


    Uniform::UniformBufferObject uniformBufferObject{};

    uniformBufferObject.modelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    uniformBufferObject.modelMatrix *= glm::rotate(glm::mat4(1.0f), passedTime * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));  // rotate around the z-axis over time.
    
    uniformBufferObject.viewMatrix = glm::lookAt(glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));  // look down at the geometry from above at a 45-degree angle.
    uniformBufferObject.projectionMatrix = glm::perspective(glm::radians(45.0f), swapchainImageExtent.width / (float)(swapchainImageExtent.height), 0.00001f, 10.0f);  // 45-degree vertical field-of-view.
    uniformBufferObject.projectionMatrix[1][1] *= -1;  // compensate for GLM's OpenGL design, invert the y-axis.


    memcpy(mappedUniformBuffersMemory[currentImage], &uniformBufferObject, sizeof(uniformBufferObject));
}
