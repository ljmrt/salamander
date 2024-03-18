#ifndef CAMERA_H
#define CAMERA_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>


namespace Camera
{
    struct ArcballCamera {
        glm::vec3 up;  // a vector pointing upwards in reference to the camera.
        glm::vec3 eye;  // where the camera is positioned.
        glm::vec3 center;  // what position the camera is looking at.

        float zoomAmount = 0.0f;  // the camera's zoom amount into the current scene.

        bool arcballEnabled = false;
        glm::vec3 initialPoint;  // the initial clicked-on point on the sphere.
        glm::quat baseQuaternion = glm::identity<glm::quat>();  // the stored non-volatile quaternion of the sphere/object before arcball.

        glm::quat volatileQuaternion = glm::identity<glm::quat>();  // the current volatile quaternion from the mouse position.

        VkExtent2D swapchainImageExtent;  // used for mouse-related function.
    };


    glm::vec3 NDCPointOnSphere(float NDCX, float NDCY);

    void glfwGetCursorNDCPosition(GLFWwindow *glfwWindow, VkExtent2D swapchainImageExtent, float& NDCX, float& NDCY);
}


#endif  // CAMERA_H
