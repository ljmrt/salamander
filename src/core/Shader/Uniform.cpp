#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <core/Shader/Uniform.h>
#include <core/Shader/Shader.h>
#include <core/DisplayManager/Camera.h>
#include <core/Buffer/Buffer.h>
#include <core/Defaults/Defaults.h>

#include <vector>
#include <cstring>
#include <algorithm>
#include <math.h>


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

void Uniform::updateFrameUniformBuffers(Uniform::UniformBuffersUpdatePackage& uniformBuffersUpdatePackage)
{
    const float nearPlane = 0.1f;
    const float farPlane = 256.0f;
    
    
    Uniform::SceneUniformBufferObject sceneUniformBufferObject{};

    sceneUniformBufferObject.modelMatrix = glm::mat4(1.0f);  // glm::rotate(glm::mat4(1.0f), passedTime * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // rotate around the y-axis over time.
    sceneUniformBufferObject.modelMatrix *= glm::mat4_cast(uniformBuffersUpdatePackage.mainMeshQuaternion);
    sceneUniformBufferObject.modelMatrix *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    sceneUniformBufferObject.modelMatrix *= glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, -0.5f));  // centers the object as we normalized the vertice position coordinates to 0..1.

    glm::quat rotationQuaternion = uniformBuffersUpdatePackage.mainCamera->baseQuaternion;
    if (uniformBuffersUpdatePackage.mainCamera->arcballEnabled == true) {
        float NDCX;
        float NDCY;
        Camera::glfwGetCursorNDCPosition(uniformBuffersUpdatePackage.glfwWindow, uniformBuffersUpdatePackage.swapchainImageExtent, NDCX, NDCY);
        
        glm::vec3 currentPoint = Camera::NDCPointOnSphere(NDCX, NDCY);
        glm::quat dragQuaternion = glm::quat(glm::dot(uniformBuffersUpdatePackage.mainCamera->initialPoint, currentPoint), glm::cross(uniformBuffersUpdatePackage.mainCamera->initialPoint, currentPoint));  // glm::quat's are stored as [w, x, y, z].
        uniformBuffersUpdatePackage.mainCamera->volatileQuaternion = dragQuaternion * uniformBuffersUpdatePackage.mainCamera->baseQuaternion;
        rotationQuaternion = uniformBuffersUpdatePackage.mainCamera->volatileQuaternion;
    }

    sceneUniformBufferObject.viewMatrix = glm::lookAt(uniformBuffersUpdatePackage.mainCamera->eye, uniformBuffersUpdatePackage.mainCamera->center, uniformBuffersUpdatePackage.mainCamera->up);  // look at the geometry head-on 3 units backwards from it's center.
    sceneUniformBufferObject.viewMatrix *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -uniformBuffersUpdatePackage.mainCamera->zoomAmount));
    sceneUniformBufferObject.viewMatrix *= glm::mat4_cast(rotationQuaternion);

    float aspectRatio = (uniformBuffersUpdatePackage.swapchainImageExtent.width / (float)(uniformBuffersUpdatePackage.swapchainImageExtent.height));

    float cameraFOV = 45.0f;  // TODO: add this as a camera struct member variable.
    sceneUniformBufferObject.projectionMatrix = glm::perspective(glm::radians(cameraFOV), aspectRatio, nearPlane, farPlane);
    sceneUniformBufferObject.projectionMatrix[1][1] *= -1;  // compensate for GLM's OpenGL design, invert the y-axis.

    sceneUniformBufferObject.normalMatrix = glm::mat4(glm::mat3(glm::transpose(glm::inverse(sceneUniformBufferObject.modelMatrix))));

    Uniform::SceneLight directionalLight;
    directionalLight.lightID = 0;  // directional light.
    directionalLight.lightProperties = glm::vec4(2.0f, 2.0f, 5.0f, 0.0f);
    directionalLight.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);

    sceneUniformBufferObject.sceneLights[0] = directionalLight;

    Uniform::SceneLight pointLight;
    pointLight.lightID = 1;  // point light.
    pointLight.lightProperties = glm::vec4(0.0f, 0.0f, -6.0f, 1.0f);
    pointLight.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.55f);

    sceneUniformBufferObject.sceneLights[1] = pointLight;

    sceneUniformBufferObject.sceneLightCount = 2;

    // TODO: light matrices do not need to be updated every frame.
    // TODO: proper variable names.
    float sizePerDepth = (atan(glm::radians(cameraFOV / 2.0f) * 2.0f));
    float distance = glm::length(glm::vec3(0, 0, 0) - uniformBuffersUpdatePackage.mainCamera->eye);
    float sizeX = (sizePerDepth * distance);
    float sizeY = (sizePerDepth * distance * aspectRatio);
    
    glm::mat4 lightProjectionMatrix = glm::ortho(-sizeX, sizeX, -sizeY, sizeY, 0.0f, (2.0f * distance));
    lightProjectionMatrix[1][1] *= -1;  // compensate for GLM's OpenGL design, invert the y-axis.
    
    glm::mat4 lightViewMatrix = glm::lookAt(glm::vec3(directionalLight.lightProperties), uniformBuffersUpdatePackage.mainCamera->center, uniformBuffersUpdatePackage.mainCamera->up);
    
    sceneUniformBufferObject.lightSpaceMatrix = (lightProjectionMatrix * lightViewMatrix);

    sceneUniformBufferObject.viewingPosition = uniformBuffersUpdatePackage.mainCamera->eye;
    
    sceneUniformBufferObject.ambientLightColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f);

    sceneUniformBufferObject.farPlane = farPlane;
    
    memcpy(uniformBuffersUpdatePackage.mappedSceneUniformBufferMemory, &sceneUniformBufferObject, sizeof(Uniform::SceneUniformBufferObject));

    
    Uniform::SceneNormalsUniformBufferObject sceneNormalsUniformBufferObject{};

    sceneNormalsUniformBufferObject.projectionMatrix = sceneUniformBufferObject.projectionMatrix;
    sceneNormalsUniformBufferObject.viewMatrix = sceneUniformBufferObject.viewMatrix;
    sceneNormalsUniformBufferObject.modelMatrix = sceneUniformBufferObject.modelMatrix;
    sceneNormalsUniformBufferObject.normalMatrix = glm::mat4(glm::mat3(glm::transpose(glm::inverse(sceneUniformBufferObject.viewMatrix * sceneUniformBufferObject.modelMatrix))));
    
    memcpy(uniformBuffersUpdatePackage.mappedSceneNormalsUniformBufferMemory, &sceneNormalsUniformBufferObject, sizeof(Uniform::SceneNormalsUniformBufferObject));


    Uniform::CubemapUniformBufferObject cubemapUniformBufferObject{};

    cubemapUniformBufferObject.projectionMatrix = sceneUniformBufferObject.projectionMatrix;
    cubemapUniformBufferObject.viewMatrix = sceneUniformBufferObject.viewMatrix;

    memcpy(uniformBuffersUpdatePackage.mappedCubemapUniformBufferMemory, &cubemapUniformBufferObject, sizeof(Uniform::CubemapUniformBufferObject));

    
    Uniform::DirectionalShadowUniformBufferObject directionalShadowUniformBufferObject{};

    directionalShadowUniformBufferObject.lightSpaceMatrix = sceneUniformBufferObject.lightSpaceMatrix;
    directionalShadowUniformBufferObject.modelMatrix = sceneUniformBufferObject.modelMatrix;
    
    memcpy(uniformBuffersUpdatePackage.mappedDirectionalShadowUniformBufferMemory, &directionalShadowUniformBufferObject, sizeof(Uniform::DirectionalShadowUniformBufferObject));


    glm::vec3 pointLightPosition = glm::vec3(pointLight.lightProperties);
    Shader::shadowTransforms[0] = glm::lookAt(pointLightPosition, (pointLightPosition + glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec3(0.0f, -1.0f, 0.0f));  // +X
    Shader::shadowTransforms[1] = glm::lookAt(pointLightPosition, (pointLightPosition + glm::vec3(-1.0f, 0.0f, 0.0f)), glm::vec3(0.0f, -1.0f, 0.0f));  // -X
    
    Shader::shadowTransforms[2] = glm::lookAt(pointLightPosition, (pointLightPosition + glm::vec3(0.0f, 1.0f, 0.0f)), glm::vec3(0.0f, 0.0f, 1.0f));  // Y+
    Shader::shadowTransforms[3] = glm::lookAt(pointLightPosition, (pointLightPosition + glm::vec3(0.0f, -1.0f, 0.0f)), glm::vec3(0.0f, 0.0f, -1.0f));  // Y-
    
    Shader::shadowTransforms[4] = glm::lookAt(pointLightPosition, (pointLightPosition + glm::vec3(0.0f, 0.0f, 1.0f)), glm::vec3(0.0f, -1.0f, 0.0f));  // Z+
    Shader::shadowTransforms[5] = glm::lookAt(pointLightPosition, (pointLightPosition + glm::vec3(0.0f, 0.0f, -1.0f)), glm::vec3(0.0f, -1.0f, 0.0f));  // Z-
    

    Uniform::PointShadowUniformBufferObject pointShadowUniformBufferObject{};

    pointShadowUniformBufferObject.projectionMatrix = glm::perspective(glm::radians(90.0f), aspectRatio, nearPlane, farPlane);
    // pointShadowUniformBufferObject.projectionMatrix[1][1] *= -1;
    pointShadowUniformBufferObject.modelMatrix = sceneUniformBufferObject.modelMatrix;

    pointShadowUniformBufferObject.pointLightPosition = pointLightPosition;
    pointShadowUniformBufferObject.farPlane = farPlane;

    memcpy(uniformBuffersUpdatePackage.mappedPointShadowUniformBufferMemory, &pointShadowUniformBufferObject, sizeof(Uniform::PointShadowUniformBufferObject));
}

void Uniform::populatePushConstant(uint32_t offset, VkDeviceSize size, VkShaderStageFlags stageFlags, VkPushConstantRange& pushConstant)
{
    pushConstant.offset = offset;
    pushConstant.size = size;
    pushConstant.stageFlags = stageFlags;
}
