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
    struct CubemapUniformBufferObject {  // identical to the cubemap shader structs.
        glm::mat4 projectionMatrix;  // identical to scene uniform buffer object projection matrix.
        glm::mat4 viewMatrix;  // identical to scene uniform buffer object view matrix.
    };

    struct SceneUniformBufferObject {  // identical to the scene shader structs.
        glm::mat4 projectionMatrix;
        glm::mat4 viewMatrix;
        glm::mat4 modelMatrix;
        glm::mat4 normalMatrix;
        glm::mat4 lightSpaceMatrix;

        glm::vec3 viewingPosition;
        // all (*)color vec4's are structered as [R, G, B, light intensity].
        alignas(16) glm::vec4 ambientLightColor;
        glm::vec3 pointLightPosition;
        alignas(16) glm::vec4 pointLightColor;
    };

    struct SceneNormalsUniformBufferObject {
        glm::mat4 projectionMatrix;
        glm::mat4 viewMatrix;
        glm::mat4 modelMatrix;
        glm::mat4 normalMatrix;
    };

    struct OffscreenUniformBufferObject {
        glm::mat4 lightMVPMatrix;
    };


    // create multiple uniform buffers(one for each frame in flight).
    //
    // @param uniformBufferObjectSize the size of the uniform buffer object that will be passed.
    // @param vulkanDevices Vulkan physical and logical device.
    // @param uniformBuffers created uniform buffers.
    // @param uniformBuffersMemory allocated uniform buffers memory.
    // @param mappedUniformBuffersMemory mapped uniform buffers memory.
    void createUniformBuffers(VkDeviceSize uniformBufferObjectSize, DeviceHandler::VulkanDevices vulkanDevices, std::vector<VkBuffer>& uniformBuffers, std::vector<VkDeviceMemory>& uniformBuffersMemory, std::vector<void *>& mappedUniformBuffersMemory);

    // update the passed frame's uniform buffers.
    //
    // @param mainCamera the scene's main camera.
    // @param meshQuaternion a quaternion to rotate the mesh.
    // @param currentImage current image/frame.
    // @param glfwWindow GLFW window to use in frame uniform buffer updating.
    // @param swapchainImageExtent Vulkan swapchain image extent.
    // @param mappedSceneUniformBuffersMemory mapped scene uniform buffers memory.
    // @param mappedSceneNormalsUniformBuffersMemory mapped scene normals uniform buffers memory.
    // @param mappedCubemapUniformBuffersMemory mapped cubemap uniform buffers memory.
    // @param mappedOffscreenUniformBuffersMemory mapped offscreen uniform buffers memory.
    void updateFrameUniformBuffers(Camera::ArcballCamera& mainCamera, glm::quat meshQuaternion, uint32_t currentImage, GLFWwindow *glfwWindow, VkExtent2D swapchainImageExtent, std::vector<void *>& mappedSceneUniformBuffersMemory, std::vector<void *>& mappedSceneNormalsUniformBuffersMemory, std::vector<void *>& mappedCubemapUniformBuffersMemory, std::vector<void *>& mappedOffscreenUniformBuffersMemory);
}


#endif  // UNIFORM_H
