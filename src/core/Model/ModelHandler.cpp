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

void ModelHandler::Model::loadModelFromAbsolutePath(std::string absoluteModelPath)
{
    absoluteModelDirectory = absoluteModelPath.substr(0, absoluteModelPath.find_last_of("/"));
    
    tinygltf::Model loadedModel;
    tinygltf::TinyGLTF modelLoader;
    std::string loaderWarnings;
    std::string loaderErrors;

    bool modelLoadingSuccess = modelLoader.LoadASCIIFromFile(&loadedModel, &loaderErrors, &loaderWarnings, absoluteModelPath.c_str());
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

        const tinygltf::Accessor& UVCoordinateAttributeAccessor = loadedModel.accessors[meshPrimitive.attributes["TEXCOORD_0"]];
        const tinygltf::BufferView& UVCoordinateAttributeBufferView = loadedModel.bufferViews[UVCoordinateAttributeAccessor.bufferView];
        const tinygltf::Buffer& UVCoordinateAttributeBuffer = loadedModel.buffers[UVCoordinateAttributeBufferView.buffer];
        
        if ((UVCoordinateAttributeAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && UVCoordinateAttributeAccessor.type == TINYGLTF_TYPE_VEC2) == false) {
            throwDebugException("Model vertices UV coordinate data is in an incorrect component type or type.");
        }
        const float *UVCoordinateAttributes = reinterpret_cast<const float *>(&UVCoordinateAttributeBuffer.data[UVCoordinateAttributeBufferView.byteOffset + UVCoordinateAttributeAccessor.byteOffset]);
        const uint32_t UV_COORDINATES_STRIDE = 2;  // UV coordinates are vec2 components.

        std::vector<ModelHandler::Vertex> primitiveVertices(positionAttributeAccessor.count);  // the amount of vertices is equivalent to the amount of positions in a primitive.
        for (size_t vertexIndex = 0; vertexIndex < positionAttributeAccessor.count; vertexIndex += 1) {
            const uint32_t VERTEX_INDEX_POSITION_OFFSET = (vertexIndex * POSITION_STRIDE);
            const float rawXPosition = positionAttributes[VERTEX_INDEX_POSITION_OFFSET + 0];
            const float rawYPosition = positionAttributes[VERTEX_INDEX_POSITION_OFFSET + 1];
            const float rawZPosition = positionAttributes[VERTEX_INDEX_POSITION_OFFSET + 2];

            // uses the X position minimum and maximum coordinates to get a general scaling factor, as individual factors would cause models to be squished into a cube.
            const float generalMinimumPositionCoordinate = positionAttributeAccessor.minValues[0];
            const float generalMaximumPositionCoordinate = positionAttributeAccessor.maxValues[0];
            primitiveVertices[vertexIndex].position.x = ModelHandler::normalizeValueToRanges(rawXPosition, generalMinimumPositionCoordinate, generalMaximumPositionCoordinate, 0, 1);
            primitiveVertices[vertexIndex].position.y = ModelHandler::normalizeValueToRanges(rawYPosition, generalMinimumPositionCoordinate, generalMaximumPositionCoordinate, 0, 1);
            primitiveVertices[vertexIndex].position.z = ModelHandler::normalizeValueToRanges(rawZPosition, generalMinimumPositionCoordinate, generalMaximumPositionCoordinate, 0, 1);
            

            const uint32_t VERTEX_INDEX_UV_COORDINATES_OFFSET = (vertexIndex * UV_COORDINATES_STRIDE);
            primitiveVertices[vertexIndex].UVCoordinates.x = UVCoordinateAttributes[VERTEX_INDEX_UV_COORDINATES_OFFSET + 0];
            primitiveVertices[vertexIndex].UVCoordinates.y = UVCoordinateAttributes[VERTEX_INDEX_UV_COORDINATES_OFFSET + 1];
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


        const tinygltf::Material modelMaterial = loadedModel.materials[0];
        const tinygltf::TextureInfo baseColorTextureInfo = modelMaterial.pbrMetallicRoughness.baseColorTexture;
        const tinygltf::Texture baseColorTexture = loadedModel.textures[baseColorTextureInfo.index];
        const tinygltf::Image baseColorTextureImage = loadedModel.images[baseColorTexture.source];
        absoluteTextureImagePath = (absoluteModelDirectory + "/" + baseColorTextureImage.uri);
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

    vkDestroySampler(vulkanLogicalDevice, textureDetails.textureSampler, nullptr);
    vkDestroyImageView(vulkanLogicalDevice, textureDetails.textureImageDetails.imageView, nullptr);
    
    vkDestroyImage(vulkanLogicalDevice, textureDetails.textureImageDetails.image, nullptr);
    vkFreeMemory(vulkanLogicalDevice, textureDetails.textureImageDetails.imageMemory, nullptr);
}

float ModelHandler::normalizeValueToRanges(float initialValue, float initialRangeMinimumValue, float initialRangeMaximumValue, float targetRangeMinimumValue, float targetRangeMaximumValue)
{
    float zeroToOneNormalizedValue = ((initialValue - initialRangeMinimumValue) / (initialRangeMaximumValue - initialRangeMinimumValue));
    return (zeroToOneNormalizedValue * ((targetRangeMaximumValue - targetRangeMinimumValue) + targetRangeMinimumValue));
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
