#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <core/Shader/Uniform.h>
#include <core/DisplayManager/Camera.h>
#include <core/Buffer/Buffer.h>
#include <core/Defaults/Defaults.h>

#include <vector>
#include <cstring>
#include <algorithm>


void Uniform::createUniformBuffers(VkDeviceSize uniformBufferObjectSize, DeviceHandler::VulkanDevices vulkanDevices, std::vector<VkBuffer>& uniformBuffers, std::vector<VkDeviceMemory>& uniformBuffersMemory, std::vector<void *>& mappedUniformBuffersMemory)
{
    uniformBuffers.resize(Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT);
    mappedUniformBuffersMemory.resize(Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT; i += 1) {
        Buffer::createBufferComponents(uniformBufferObjectSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vulkanDevices, uniformBuffers[i], uniformBuffersMemory[i]);

        vkMapMemory(vulkanDevices.logicalDevice, uniformBuffersMemory[i], 0, uniformBufferObjectSize, 0, &mappedUniformBuffersMemory[i]);
    }
}

void Uniform::updateFrameUniformBuffers(Camera::ArcballCamera& mainCamera, glm::quat meshQuaternion, uint32_t currentImage, GLFWwindow *glfwWindow, VkExtent2D swapchainImageExtent, std::vector<void *>& mappedSceneUniformBuffersMemory, std::vector<void *>& mappedSceneNormalsUniformBuffersMemory, std::vector<void *>& mappedCubemapUniformBuffersMemory, std::vector<void *>& mappedOffscreenUniformBuffersMemory)
{
    const float nearPlane = 0.1f;
    const float farPlane = 256.0f;
    
    
    Uniform::SceneUniformBufferObject sceneUniformBufferObject{};

    sceneUniformBufferObject.modelMatrix = glm::mat4(1.0f);  // glm::rotate(glm::mat4(1.0f), passedTime * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // rotate around the y-axis over time.
    sceneUniformBufferObject.modelMatrix *= glm::mat4_cast(meshQuaternion);
    sceneUniformBufferObject.modelMatrix *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    sceneUniformBufferObject.modelMatrix *= glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, -0.5f));  // centers the object as we normalized the vertice position coordinates to 0..1.

    glm::quat rotationQuaternion = mainCamera.baseQuaternion;
    if (mainCamera.arcballEnabled == true) {
        float NDCX;
        float NDCY;
        Camera::glfwGetCursorNDCPosition(glfwWindow, swapchainImageExtent, NDCX, NDCY);
        
        glm::vec3 currentPoint = Camera::NDCPointOnSphere(NDCX, NDCY);
        glm::quat dragQuaternion = glm::quat(glm::dot(mainCamera.initialPoint, currentPoint), glm::cross(mainCamera.initialPoint, currentPoint));  // glm::quat's are stored as [w, x, y, z].
        mainCamera.volatileQuaternion = dragQuaternion * mainCamera.baseQuaternion;
        rotationQuaternion = mainCamera.volatileQuaternion;
    }

    sceneUniformBufferObject.viewMatrix = glm::lookAt(mainCamera.eye, mainCamera.center, mainCamera.up);  // look at the geometry head-on 3 units backwards from it's center.
    sceneUniformBufferObject.viewMatrix *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -mainCamera.zoomAmount));
    sceneUniformBufferObject.viewMatrix *= glm::mat4_cast(rotationQuaternion);

    sceneUniformBufferObject.projectionMatrix = glm::perspective(glm::radians(45.0f), swapchainImageExtent.width / (float)(swapchainImageExtent.height), nearPlane, farPlane);
    sceneUniformBufferObject.projectionMatrix[1][1] *= -1;  // compensate for GLM's OpenGL design, invert the y-axis.

    sceneUniformBufferObject.normalMatrix = glm::mat4(glm::mat3(glm::transpose(glm::inverse(sceneUniformBufferObject.modelMatrix))));

    glm::mat4 lightViewMatrix = glm::lookAt(sceneUniformBufferObject.pointLightPosition, mainCamera.center, mainCamera.up);
    sceneUniformBufferObject.lightSpaceMatrix = (sceneUniformBufferObject.projectionMatrix * lightViewMatrix * sceneUniformBufferObject.modelMatrix);

    sceneUniformBufferObject.viewingPosition = mainCamera.eye;
    sceneUniformBufferObject.ambientLightColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f);
    sceneUniformBufferObject.pointLightPosition = glm::vec3(0.0f, 0.0f, -1.0f);  // this is in world space.
    sceneUniformBufferObject.pointLightColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.25f);
    
    memcpy(mappedSceneUniformBuffersMemory[currentImage], &sceneUniformBufferObject, sizeof(Uniform::SceneUniformBufferObject));

    
    Uniform::SceneNormalsUniformBufferObject sceneNormalsUniformBufferObject{};

    sceneNormalsUniformBufferObject.projectionMatrix = sceneUniformBufferObject.projectionMatrix;
    sceneNormalsUniformBufferObject.viewMatrix = sceneUniformBufferObject.viewMatrix;
    sceneNormalsUniformBufferObject.modelMatrix = sceneUniformBufferObject.modelMatrix;
    sceneNormalsUniformBufferObject.normalMatrix = glm::mat4(glm::mat3(glm::transpose(glm::inverse(sceneUniformBufferObject.viewMatrix * sceneUniformBufferObject.modelMatrix))));
    
    memcpy(mappedSceneNormalsUniformBuffersMemory[currentImage], &sceneNormalsUniformBufferObject, sizeof(Uniform::SceneNormalsUniformBufferObject));


    Uniform::CubemapUniformBufferObject cubemapUniformBufferObject{};

    cubemapUniformBufferObject.projectionMatrix = sceneUniformBufferObject.projectionMatrix;
    cubemapUniformBufferObject.viewMatrix = sceneUniformBufferObject.viewMatrix;

    memcpy(mappedCubemapUniformBuffersMemory[currentImage], &cubemapUniformBufferObject, sizeof(Uniform::CubemapUniformBufferObject));

    
    Uniform::OffscreenUniformBufferObject offscreenUniformBufferObject{};

    offscreenUniformBufferObject.lightMVPMatrix = (sceneUniformBufferObject.lightSpaceMatrix * sceneUniformBufferObject.modelMatrix);
    
    memcpy(mappedOffscreenUniformBuffersMemory[currentImage], &offscreenUniformBufferObject, sizeof(Uniform::OffscreenUniformBufferObject));
}
