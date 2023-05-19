#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION  // defined in tinygltf as it is the only user and comes pre-included.
#define TINYGLTF_USE_CPP14
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#include <tinygltf/tiny_gltf.h>

#include <core/Model/ModelHandler.h>
#include <core/Shader/ResourceDescriptor.h>
#include <core/Buffer/Buffer.h>
#include <core/VulkanInstance/DeviceHandler.h>
#include <core/Logging/ErrorLogger.h>

#include <array>
#include <vector>
#include <string>
#include <iostream>


std::array<VkVertexInputAttributeDescription, 3> ModelHandler::preservedAttributeDescriptions;
VkVertexInputBindingDescription ModelHandler::preservedBindingDescription;

void ModelHandler::Model::loadModelFromPath(std::string modelPath)
{
    tinygltf::Model loadedModel;
    tinygltf::TinyGLTF modelLoader;
    std::string loaderWarnings;
    std::string loaderErrors;

    bool modelLoadingSuccess = modelLoader.LoadASCIIFromFile(&loadedModel, &loaderErrors, &loaderWarnings, modelPath.c_str());
    if (modelLoadingSuccess == false) {
        throwDebugException("Failed to load model/parse glTF.");
    }

    for (tinygltf::Primitive meshPrimitive : loadedModel.meshes[0].primitives) {  // only supports one mesh?
        const tinygltf::Accessor& positionAttributeAccessor = loadedModel.accessors[meshPrimitive.attributes["POSITION"]];
        const tinygltf::BufferView& positionAttributeBufferView = loadedModel.bufferViews[positionAttributeAccessor.bufferView];
        const tinygltf::Buffer& positionAttributeBuffer = loadedModel.buffers[positionAttributeBufferView.buffer];

        if ((positionAttributeAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && positionAttributeAccessor.type == TINYGLTF_TYPE_VEC3) == false) {
            throwDebugException("Model vertices position data is in an incorrect component type or type.");
        }
        const float *positionAttributes = reinterpret_cast<const float *>(&positionAttributeBuffer.data[positionAttributeBufferView.byteOffset + positionAttributeAccessor.byteOffset]);  // get the position attribute data from the buffer starting at the actual data offset to the end of the buffer(position attributes are only up to positionAttributeAccessor.count multiplied by the entire data type stride).
        const uint32_t POSITION_STRIDE = 3;  // positions are vec3 components.

        std::vector<ModelHandler::Vertex> primitiveVertices(positionAttributeAccessor.count);
        for (size_t positionIndex = 0; positionIndex < positionAttributeAccessor.count; positionIndex += 1) {
            // position attribute data is packed tightly.
            const uint32_t POSITION_INDEX_OFFSET = (positionIndex * POSITION_STRIDE);
            primitiveVertices[positionIndex].position.x = positionAttributes[POSITION_INDEX_OFFSET + 0];
            primitiveVertices[positionIndex].position.y = positionAttributes[POSITION_INDEX_OFFSET + 1];
            primitiveVertices[positionIndex].position.z = positionAttributes[POSITION_INDEX_OFFSET + 2];
        }

        meshVertices.insert(meshVertices.end(), primitiveVertices.begin(), primitiveVertices.end());
        

        const tinygltf::Accessor& indicesAccessor = loadedModel.accessors[meshPrimitive.indices];
        const tinygltf::BufferView& indicesBufferView = loadedModel.bufferViews[indicesAccessor.bufferView];
        const tinygltf::Buffer& indicesBuffer = loadedModel.buffers[indicesBufferView.buffer];

        if ((indicesAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT && indicesAccessor.type == TINYGLTF_TYPE_SCALAR) == false) {
            throwDebugException("Model indices data is in an incorrect component type or type.");
        }
        const uint16_t *indices = reinterpret_cast<const uint16_t *>(&indicesBuffer.data[indicesBufferView.byteOffset + indicesAccessor.byteOffset]);
        std::vector<uint32_t> primitiveIndices(indices, (indices + indicesAccessor.count));

        meshIndices.insert(meshIndices.end(), primitiveIndices.begin(), primitiveIndices.end());
    }
}

void ModelHandler::Model::createModelBuffers(VkCommandPool commandPool, VkQueue commandQueue, DeviceHandler::VulkanDevices vulkanDevices)
{
    Buffer::createDataBufferComponents(meshVertices.data(), (sizeof(meshVertices[0]) * meshVertices.size()), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, commandPool, commandQueue, vulkanDevices, vertexBuffer, vertexBufferMemory);
    Buffer::createDataBufferComponents(meshIndices.data(), (sizeof(meshIndices[0]) * meshIndices.size()), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, commandPool, commandQueue, vulkanDevices, indexBuffer, indexBufferMemory);
}

void ModelHandler::Model::cleanupModel(VkDevice vulkanLogicalDevice)
{
    vkDestroyBuffer(vulkanLogicalDevice, vertexBuffer, nullptr);
    vkFreeMemory(vulkanLogicalDevice, vertexBufferMemory, nullptr);

    vkDestroyBuffer(vulkanLogicalDevice, indexBuffer, nullptr);
    vkFreeMemory(vulkanLogicalDevice, indexBufferMemory, nullptr);
}

void ModelHandler::populateVertexInputCreateInfo(VkPipelineVertexInputStateCreateInfo& vertexInputCreateInfo)
{
    vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    ResourceDescriptor::populateBindingDescription(preservedBindingDescription);

    ResourceDescriptor::fetchAttributeDescriptions(preservedAttributeDescriptions);
    
    vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputCreateInfo.pVertexBindingDescriptions = &preservedBindingDescription;
    vertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(preservedAttributeDescriptions.size());
    vertexInputCreateInfo.pVertexAttributeDescriptions = preservedAttributeDescriptions.data();
}

void ModelHandler::populateInputAssemblyCreateInfo(VkPipelineInputAssemblyStateCreateInfo& inputAssemblyCreateInfo)
{
    inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    
    inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;  // form triangle primitives without vertex reuse.
    inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;  // disable the possibility of vertex reuse.
}
