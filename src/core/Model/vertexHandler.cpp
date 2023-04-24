#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Model/vertexHandler.h>

#include <glm/glm.hpp>

#include <vector>


static void vertexHandler::Vertex::fetchBindingDescription(VkVertexInputBindingDescription& bindingDescription)
{
    bindingDescription.binding = 0;  // binding index in the "array" of bindings.
    bindingDescription.stride = sizeof(vertexHandler::Vertex);  // byte distance from one entry to another.
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}

static void fetchAttributeDescriptions(std::vector<VkVertexInputAttributeDescription, 2>& attributeDescriptions)
{
    // position description.
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;  // "location" keyword in shader.
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(vertexHandler::Vertex, position);

    // color description.
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;  // "location" keyword in shader.
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(vertexHandler::Vertex, color);
}
