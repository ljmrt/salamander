#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <core/DisplayManager/Camera.h>

#include <math.h>


glm::vec3 Camera::NDCPointOnSphere(float NDCX, float NDCY)
{
    // modified Shoemake arcball projection, removed unnecessary equations(we use some fixed variables).
    float distance = ((NDCX * NDCX) + (NDCY * NDCY));
    if (distance > 1.0f) {  // to-be point is outside of the sphere.
        // "project" the point onto the sphere.
        float normalizedNDCX = (NDCX / sqrt(distance));
        float normalizedNDCY = (NDCY / sqrt(distance));
        return glm::vec3(normalizedNDCX, normalizedNDCY, 0.0f);
    } else {  // to-be point is already on the sphere.
        return glm::vec3(NDCX, NDCY, sqrt(1.0f - distance));
    }
}

void Camera::glfwGetCursorNDCPosition(GLFWwindow *glfwWindow, VkExtent2D swapchainImageExtent, float& NDCX, float& NDCY)
{
    double rawX;
    double rawY;
    glfwGetCursorPos(glfwWindow, &rawX, &rawY);
    
    NDCX = (((rawX / swapchainImageExtent.width) - 0.5) * 2);
    NDCY = (((rawY / swapchainImageExtent.height) - 0.5) * 2);            
}
