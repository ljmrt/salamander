#ifndef VERTEXHANDLER_H
#define VERTEXHANDLER_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <vector>
#include <array>


namespace VertexHandler
{
    struct Vertex {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec2 textureCoordinates;
    };

    const std::vector<Vertex> vertices = {  // represents the vertex buffer's data.
        // position(vec3), color(vec3), texture coordinates(vec2).
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
    };

    const std::vector<uint16_t> indices = {  // represents the index buffer.
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4
    };

    extern std::array<VkVertexInputAttributeDescription, 3> preservedAttributeDescriptions;  // preserved attribute descriptions(pointer reasons).
    extern VkVertexInputBindingDescription preservedBindingDescription;  // preserved binding description(pointer reasons).


    // populate a vertex input's create info.
    //
    // @param vertexInputCreateInfo stored filled vertex input create info.
    void populateVertexInputCreateInfo(VkPipelineVertexInputStateCreateInfo& vertexInputCreateInfo);

    // populate a input assembly's create info.
    //
    // @param inputAssemblyCreateInfo stored filled input assembly create info.
    void populateInputAssemblyCreateInfo(VkPipelineInputAssemblyStateCreateInfo& inputAssemblyCreateInfo);
}


#endif  // VERTEXHANDLER_H
