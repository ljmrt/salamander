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
    struct SceneLight {
        uint32_t lightID;  // 0: directional, 1: point, 2: spotlight(not implemented, yet).
        alignas(16) glm::vec4 lightProperties;  // used as a direction/position distinguished by the light ID.
        glm::vec4 lightColor;  // structured as [R, G, B, light intensity].
    };
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
        
        alignas(16) glm::vec4 ambientLightColor;  // structered as [R, G, B, light intensity].

        // TODO: implement a spotlight light-caster.
        Uniform::SceneLight sceneLights[128];
        uint32_t sceneLightCount;
    };

    struct SceneNormalsUniformBufferObject {
        glm::mat4 projectionMatrix;
        glm::mat4 viewMatrix;
        glm::mat4 modelMatrix;
        glm::mat4 normalMatrix;
    };

    struct DirectionalShadowUniformBufferObject {
        glm::mat4 lightSpaceMatrix;
        glm::mat4 modelMatrix;
    };

    struct PointShadowUniformBufferObject {
        glm::mat4 shadowTransforms[6];  // "shadow transforms" for all the 6 faces of a cubemap.
        glm::mat4 modelMatrix;        
    };
    
    struct UniformBuffersUpdatePackage {
        Camera::ArcballCamera *mainCamera;  // the scene's main camera.
        glm::quat mainMeshQuaternion;  // the main mesh's provided optional quaternion.

        VkExtent2D swapchainImageExtent;  // Vulkan swapchain image extent.        
        GLFWwindow *glfwWindow;  // GLFW window to use in frame uniform buffer updating.

        // uniform buffer at the index of the current frame.
        void *mappedSceneUniformBufferMemory;  // mapped scene uniform buffer memory.
        void *mappedSceneNormalsUniformBufferMemory;  // mapped scene normals uniform buffer memory.
        void *mappedCubemapUniformBufferMemory;  // mapped cubemap uniform buffer memory.
        void *mappedDirectionalShadowUniformBufferMemory;  // mapped directional shadow uniform buffer memory.
        void *mappedPointShadowUniformBufferMemory;  // mapped point shadow uniform buffer memory.
    };


    // create multiple uniform buffers(one for each frame in flight).
    //
    // @param uniformBufferObjectSize the size of the uniform buffer object that will be passed.
    // @param vulkanDevices Vulkan physical and logical device.
    // @param uniformBuffers created uniform buffers.
    // @param uniformBuffersMemory allocated uniform buffers memory.
    // @param mappedUniformBuffersMemory mapped uniform buffers memory.
    void createUniformBuffers(VkDeviceSize uniformBufferObjectSize, DeviceHandler::VulkanDevices vulkanDevices, std::vector<VkBuffer>& uniformBuffers, std::vector<VkDeviceMemory>& uniformBuffersMemory, std::vector<void *>& mappedUniformBuffersMemory);

    // update the frame's uniform buffers.
    //
    // @param uniformBuffersUpdatePackage the uniform buffers update data to use in this function.
    void updateFrameUniformBuffers(Uniform::UniformBuffersUpdatePackage& uniformBuffersUpdatePackage);
}


#endif  // UNIFORM_H
