#ifndef UNIFORM_H
#define UNIFORM_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <core/DisplayManager/Camera.h>
#include <core/VulkanInstance/DeviceHandler.h>

#include <vector>


namespace Uniform
{
    struct UniformBufferObject {  // identical to the vertex shader's struct.
        glm::mat4 pvMatrix;
        glm::mat4 modelMatrix;
        glm::mat3 normalMatrix;

        // all (*)color vec4's are structered as [R, G, B, light intensity].
        glm::vec4 ambientLightColor;
        glm::vec3 pointLightPosition;
        glm::vec4 pointLightColor;
    };


    // create multiple uniform buffers(one for each frame in flight).
    //
    // @param vulkanDevices Vulkan physical and logical device.
    // @param uniformBuffers created uniform buffers.
    // @param uniformBuffersMemory allocated uniform buffers memory.
    // @param mappedUniformBuffersMemory mapped uniform buffers memory.
    void createUniformBuffers(DeviceHandler::VulkanDevices vulkanDevices, std::vector<VkBuffer>& uniformBuffers, std::vector<VkDeviceMemory>& uniformBuffersMemory, std::vector<void *>& mappedUniformBuffersMemory);

    // update the passed frame's vertex shader uniform buffer.
    //
    // @param mainCamera the scene's main camera.
    // @param meshQuaternion a quaternion to rotate the mesh.
    // @param currentImage current image/frame.
    // @param glfwWindow GLFW window to use in frame uniform buffer updating.
    // @param swapchainImageExtent Vulkan swapchain image extent.
    // @param mappedUniformBuffersMemory mapped uniform buffers memory.
    void updateFrameUniformBuffer(Camera::ArcballCamera& mainCamera, glm::quat meshQuaternion, size_t currentImage, GLFWwindow *glfwWindow, VkExtent2D swapchainImageExtent, std::vector<void *>& mappedUniformBuffersMemory);
}


#endif  // UNIFORM_H
