#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Model/VertexHandler.h>
#include <core/Shader/ResourceDescriptor.h>

#include <array>


std::array<VkVertexInputAttributeDescription, 3> VertexHandler::preservedAttributeDescriptions;
VkVertexInputBindingDescription VertexHandler::preservedBindingDescription;

void VertexHandler::populateVertexInputCreateInfo(VkPipelineVertexInputStateCreateInfo& vertexInputCreateInfo)
{
    vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    ResourceDescriptor::populateBindingDescription(preservedBindingDescription);

    ResourceDescriptor::fetchAttributeDescriptions(preservedAttributeDescriptions);
    
    vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputCreateInfo.pVertexBindingDescriptions = &preservedBindingDescription;
    vertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(preservedAttributeDescriptions.size());
    vertexInputCreateInfo.pVertexAttributeDescriptions = preservedAttributeDescriptions.data();
}

void VertexHandler::populateInputAssemblyCreateInfo(VkPipelineInputAssemblyStateCreateInfo& inputAssemblyCreateInfo)
{
    inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    
    inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;  // form triangle primitives without vertex reuse.
    inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;  // disable the possibility of vertex reuse.
}
