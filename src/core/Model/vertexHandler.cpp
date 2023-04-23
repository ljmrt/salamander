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
