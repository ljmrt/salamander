#ifndef VERTEXHANDLER_H
#define VERTEXHANDLER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <vector>


namespace vertexHandler
{
    struct Vertex {
        glm::vec2 position;
        glm::vec3 color;

        static void fetchBindingDescription(VkVertexInputBindingDescription& bindingDescription);
    };

    const std::vector<Vertex> vertices = {
        // position(vec2), color(vec3).
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };
}


#endif  // VERTEXHANDLER_H
