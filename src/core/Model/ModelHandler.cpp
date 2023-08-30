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
#include <utils/MathUtils.h>

#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <string.h>


void ModelHandler::Model::loadModelFromAbsolutePath(std::string absoluteModelPath)
{
    this->absoluteModelDirectory = absoluteModelPath.substr(0, absoluteModelPath.find_last_of("/"));
    
    tinygltf::Model loadedModel;
    tinygltf::TinyGLTF modelLoader;
    std::string loaderWarnings;
    std::string loaderErrors;

    bool modelLoadingSuccess = modelLoader.LoadASCIIFromFile(&loadedModel, &loaderErrors, &loaderWarnings, absoluteModelPath.c_str());
    if (modelLoadingSuccess == false) {
        std::cout << loaderWarnings << std::endl;
        std::cout << loaderErrors << std::endl;
        throwDebugException("Failed to load model/parse glTF.");
    }
    for (tinygltf::Mesh selectedMesh : loadedModel.meshes) {
        for (tinygltf::Primitive meshPrimitive : selectedMesh.primitives) {
            const tinygltf::Accessor& positionAttributeAccessor = loadedModel.accessors[meshPrimitive.attributes["POSITION"]];
            const tinygltf::BufferView& positionAttributeBufferView = loadedModel.bufferViews[positionAttributeAccessor.bufferView];
            const tinygltf::Buffer& positionAttributeBuffer = loadedModel.buffers[positionAttributeBufferView.buffer];

            if ((positionAttributeAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && positionAttributeAccessor.type == TINYGLTF_TYPE_VEC3) == false) {
                throwDebugException("Model vertices position data is in an incorrect component type or type.");
            }
            const float *positionAttributes = reinterpret_cast<const float *>(&positionAttributeBuffer.data[positionAttributeBufferView.byteOffset + positionAttributeAccessor.byteOffset]);  // get the position attribute data from the buffer starting at the actual data offset to the end of the buffer(position attributes are only up to positionAttributeAccessor.count multiplied by the entire data type stride).
            const uint32_t POSITION_STRIDE = 3;  // positions are vec3 components.

            const tinygltf::Accessor& normalAttributeAccessor = loadedModel.accessors[meshPrimitive.attributes["NORMAL"]];
            const tinygltf::BufferView& normalAttributeBufferView = loadedModel.bufferViews[normalAttributeAccessor.bufferView];
            const tinygltf::Buffer& normalAttributeBuffer = loadedModel.buffers[normalAttributeBufferView.buffer];
        
            if ((normalAttributeAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && normalAttributeAccessor.type == TINYGLTF_TYPE_VEC3) == false) {
                throwDebugException("Model normal data is in an incorrect component type or type.");
            }
            const float *normalAttributes = reinterpret_cast<const float *>(&normalAttributeBuffer.data[normalAttributeBufferView.byteOffset + normalAttributeAccessor.byteOffset]);
            const uint32_t NORMAL_STRIDE = 3;  // normals are vec3 components.

            const tinygltf::Accessor& tangentAttributeAccessor = loadedModel.accessors[meshPrimitive.attributes["TANGENT"]];
            const tinygltf::BufferView& tangentAttributeBufferView = loadedModel.bufferViews[tangentAttributeAccessor.bufferView];
            const tinygltf::Buffer& tangentAttributeBuffer = loadedModel.buffers[tangentAttributeBufferView.buffer];
        
            if ((tangentAttributeAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && (tangentAttributeAccessor.type == TINYGLTF_TYPE_VEC3 || tangentAttributeAccessor.type == TINYGLTF_TYPE_VEC4)) == false) {
                throwDebugException("Model tangent normal data is in an incorrect component type or type.");
            }
            const float *tangentAttributes = reinterpret_cast<const float *>(&tangentAttributeBuffer.data[tangentAttributeBufferView.byteOffset + tangentAttributeAccessor.byteOffset]);
            const uint32_t TANGENT_STRIDE = 3;  // tangents are vec3 components.

            const tinygltf::Accessor& UVCoordinateAttributeAccessor = loadedModel.accessors[meshPrimitive.attributes["TEXCOORD_0"]];
            const tinygltf::BufferView& UVCoordinateAttributeBufferView = loadedModel.bufferViews[UVCoordinateAttributeAccessor.bufferView];
            const tinygltf::Buffer& UVCoordinateAttributeBuffer = loadedModel.buffers[UVCoordinateAttributeBufferView.buffer];
        
            if ((UVCoordinateAttributeAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && UVCoordinateAttributeAccessor.type == TINYGLTF_TYPE_VEC2) == false) {
                std::cout << UVCoordinateAttributeAccessor.componentType << " : " << UVCoordinateAttributeAccessor.type << std::endl;
                throwDebugException("Model vertices UV coordinate data is in an incorrect component type or type.");
            }
            const float *UVCoordinateAttributes = reinterpret_cast<const float *>(&UVCoordinateAttributeBuffer.data[UVCoordinateAttributeBufferView.byteOffset + UVCoordinateAttributeAccessor.byteOffset]);
            const uint32_t UV_COORDINATES_STRIDE = 2;  // UV coordinates are vec2 components.

            std::vector<ModelHandler::SceneVertexData> primitiveVertices(positionAttributeAccessor.count);  // the amount of vertices is equivalent to the amount of positions in a primitive.
            for (size_t vertexIndex = 0; vertexIndex < positionAttributeAccessor.count; vertexIndex += 1) {
                const uint32_t VERTEX_INDEX_POSITION_OFFSET = (vertexIndex * POSITION_STRIDE);
                const float rawXPosition = positionAttributes[VERTEX_INDEX_POSITION_OFFSET + 0];
                const float rawYPosition = positionAttributes[VERTEX_INDEX_POSITION_OFFSET + 1];
                float rawZPosition = positionAttributes[VERTEX_INDEX_POSITION_OFFSET + 2];
                if (rawZPosition > -1 && rawZPosition < -0.999) {  // correct strange values.
                    rawZPosition = -1;
                }

                // uses the X position minimum and maximum coordinates to get a general scaling factor, as individual factors would cause models to be squished into a cube.
                const float generalMinimumPositionCoordinate = positionAttributeAccessor.minValues[0];
                const float generalMaximumPositionCoordinate = positionAttributeAccessor.maxValues[0];
                primitiveVertices[vertexIndex].position.x = MathUtils::normalizeValueToRanges(rawXPosition, generalMinimumPositionCoordinate, generalMaximumPositionCoordinate, 0, 1);
                primitiveVertices[vertexIndex].position.y = MathUtils::normalizeValueToRanges(rawYPosition, generalMinimumPositionCoordinate, generalMaximumPositionCoordinate, 0, 1);
                primitiveVertices[vertexIndex].position.z = MathUtils::normalizeValueToRanges(rawZPosition, generalMinimumPositionCoordinate, generalMaximumPositionCoordinate, 0, 1);
                if (strcmp(loadedModel.nodes[0].name.c_str(), "Cube") == 0) {  // if the model's first node's name is "Cube".
                    // this is the cubemap model, scale all positions up.
                    primitiveVertices[vertexIndex].position.x = rawXPosition * 256;
                    primitiveVertices[vertexIndex].position.y = rawYPosition * 256;
                    primitiveVertices[vertexIndex].position.z = rawZPosition * 256;
                }
            

                const uint32_t VERTEX_INDEX_NORMAL_OFFSET = (vertexIndex * NORMAL_STRIDE);
                primitiveVertices[vertexIndex].normal.x = normalAttributes[VERTEX_INDEX_NORMAL_OFFSET + 1];
                primitiveVertices[vertexIndex].normal.y = normalAttributes[VERTEX_INDEX_NORMAL_OFFSET + 1];
                primitiveVertices[vertexIndex].normal.z = normalAttributes[VERTEX_INDEX_NORMAL_OFFSET + 2];


                const uint32_t VERTEX_INDEX_TANGENT_OFFSET = (vertexIndex * TANGENT_STRIDE);
                float extractedTangentX = tangentAttributes[VERTEX_INDEX_TANGENT_OFFSET + 1];
                float extractedTangentY = tangentAttributes[VERTEX_INDEX_TANGENT_OFFSET + 1];
                float extractedTangentZ = tangentAttributes[VERTEX_INDEX_TANGENT_OFFSET + 2];
                float extractedTangentW = 1;  // Only modified under the case in which the tangents are specified as vec4's.
                
                if (tangentAttributeAccessor.type == TINYGLTF_TYPE_VEC4) {
                    extractedTangentW = tangentAttributes[VERTEX_INDEX_TANGENT_OFFSET + 3];
                }
                primitiveVertices[vertexIndex].tangent = glm::vec3((extractedTangentX / extractedTangentW), (extractedTangentY / extractedTangentW), (extractedTangentZ / extractedTangentW));


                const uint32_t VERTEX_INDEX_UV_COORDINATES_OFFSET = (vertexIndex * UV_COORDINATES_STRIDE);
                primitiveVertices[vertexIndex].UVCoordinates.x = UVCoordinateAttributes[VERTEX_INDEX_UV_COORDINATES_OFFSET + 0];
                primitiveVertices[vertexIndex].UVCoordinates.y = UVCoordinateAttributes[VERTEX_INDEX_UV_COORDINATES_OFFSET + 1];
            }

            this->meshVertices.insert(this->meshVertices.end(), primitiveVertices.begin(), primitiveVertices.end());
        

            if (meshPrimitive.indices != -1) {  // -1 indicates that there is no indices.
                const tinygltf::Accessor& indicesAccessor = loadedModel.accessors[meshPrimitive.indices];
                const tinygltf::BufferView& indicesBufferView = loadedModel.bufferViews[indicesAccessor.bufferView];
                const tinygltf::Buffer& indicesBuffer = loadedModel.buffers[indicesBufferView.buffer];

                if (((indicesAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT || indicesAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) && indicesAccessor.type == TINYGLTF_TYPE_SCALAR) == false) {
                    throwDebugException("Model indices data is in an incorrect component type or type.");
                }
                const uint16_t *indices = reinterpret_cast<const uint16_t *>(&indicesBuffer.data[indicesBufferView.byteOffset + indicesAccessor.byteOffset]);
                std::vector<uint32_t> primitiveIndices(indices, (indices + indicesAccessor.count));

                this->meshIndices.insert(this->meshIndices.end(), primitiveIndices.begin(), primitiveIndices.end());
            }


            const tinygltf::Material modelMaterial = loadedModel.materials[0];
            
            const int32_t baseColorTextureIndex = modelMaterial.pbrMetallicRoughness.baseColorTexture.index;
            if (baseColorTextureIndex != -1) {  // -1 indicates there is no base color texture.
                const tinygltf::Texture baseColorTexture = loadedModel.textures[baseColorTextureIndex];
                const tinygltf::Image baseColorTextureImage = loadedModel.images[baseColorTexture.source];
                this->absoluteTextureImagePath = (this->absoluteModelDirectory + "/" + baseColorTextureImage.uri);
            } else {
                this->absoluteTextureImagePath = "NOT AVAILIBLE";
            }

            const int32_t normalTextureIndex = modelMaterial.normalTexture.index;
            if (normalTextureIndex != -1) {  // if there is a normal texture index supplied.
                const tinygltf::Texture normalTexture = loadedModel.textures[normalTextureIndex];
                const tinygltf::Image normalTextureImage = loadedModel.images[normalTexture.source];
                this->absoluteNormalImagePath = (this->absoluteModelDirectory + "/" + normalTextureImage.uri);
            } else {
                this->absoluteNormalImagePath = "NOT AVAILABLE";
            }
        }
    }
}

void ModelHandler::Model::normalizeNormalValues()
{
    // taken from the minimum and maximum normal X coordinates
    float generalMinimumNormalValue = 100000;
    float generalMaximumNormalValue = 0;
    for (ModelHandler::SceneVertexData meshVertex : this->meshVertices) {
        if (meshVertex.normal.x < generalMinimumNormalValue) {
            generalMinimumNormalValue = meshVertex.normal.x;
        }
        if (meshVertex.normal.x > generalMaximumNormalValue) {
            generalMaximumNormalValue = meshVertex.normal.x;
        }
    }

    for (ModelHandler::SceneVertexData meshVertex : this->meshVertices) {
        meshVertex.normal.x = MathUtils::normalizeValueToRanges(meshVertex.normal.x, generalMinimumNormalValue, generalMaximumNormalValue, 0, 1);
        meshVertex.normal.y = MathUtils::normalizeValueToRanges(meshVertex.normal.y, generalMinimumNormalValue, generalMaximumNormalValue, 0, 1);
        meshVertex.normal.z = MathUtils::normalizeValueToRanges(meshVertex.normal.z, generalMinimumNormalValue, generalMaximumNormalValue, 0, 1);
    }
}

void ModelHandler::Model::populateShaderBufferComponents(std::vector<ModelHandler::SceneVertexData> vertexData, VkCommandPool commandPool, VkQueue commandQueue, DeviceHandler::VulkanDevices vulkanDevices)
{
    Buffer::createDataBufferComponents(vertexData.data(), (sizeof(vertexData[0]) * vertexData.size()), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, commandPool, commandQueue, vulkanDevices, this->shaderBufferComponents.vertexBuffer, this->shaderBufferComponents.vertexBufferMemory);

    if (this->meshIndices.empty() == false) {
        Buffer::createDataBufferComponents(this->meshIndices.data(), (sizeof(this->meshIndices[0]) * this->meshIndices.size()), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, commandPool, commandQueue, vulkanDevices, this->shaderBufferComponents.indexBuffer, this->shaderBufferComponents.indexBufferMemory);

        shaderBufferComponents.verticeCount = -1;
        shaderBufferComponents.indiceCount = static_cast<int32_t>(this->meshIndices.size());
    } else {
        shaderBufferComponents.verticeCount = static_cast<uint32_t>(this->meshVertices.size());
        shaderBufferComponents.indiceCount = -1;
    }
}

void ModelHandler::Model::populateShaderBufferComponents(std::vector<ModelHandler::SceneNormalsVertexData> vertexData, VkCommandPool commandPool, VkQueue commandQueue, DeviceHandler::VulkanDevices vulkanDevices)
{
    Buffer::createDataBufferComponents(vertexData.data(), (sizeof(vertexData[0]) * vertexData.size()), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, commandPool, commandQueue, vulkanDevices, this->shaderBufferComponents.vertexBuffer, this->shaderBufferComponents.vertexBufferMemory);

    if (this->meshIndices.empty() == false) {
        Buffer::createDataBufferComponents(this->meshIndices.data(), (sizeof(this->meshIndices[0]) * this->meshIndices.size()), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, commandPool, commandQueue, vulkanDevices, this->shaderBufferComponents.indexBuffer, this->shaderBufferComponents.indexBufferMemory);

        shaderBufferComponents.verticeCount = -1;
        shaderBufferComponents.indiceCount = static_cast<int32_t>(this->meshIndices.size());
    } else {
        shaderBufferComponents.verticeCount = static_cast<uint32_t>(this->meshVertices.size());
        shaderBufferComponents.indiceCount = -1;
    }
}

void ModelHandler::Model::populateShaderBufferComponents(std::vector<ModelHandler::CubemapVertexData> vertexData, VkCommandPool commandPool, VkQueue commandQueue, DeviceHandler::VulkanDevices vulkanDevices)
{
    Buffer::createDataBufferComponents(vertexData.data(), (sizeof(vertexData[0]) * vertexData.size()), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, commandPool, commandQueue, vulkanDevices, this->shaderBufferComponents.vertexBuffer, this->shaderBufferComponents.vertexBufferMemory);

    if (this->meshIndices.empty() == false) {
        Buffer::createDataBufferComponents(this->meshIndices.data(), (sizeof(this->meshIndices[0]) * this->meshIndices.size()), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, commandPool, commandQueue, vulkanDevices, this->shaderBufferComponents.indexBuffer, this->shaderBufferComponents.indexBufferMemory);
        
        shaderBufferComponents.verticeCount = -1;
        shaderBufferComponents.indiceCount = static_cast<uint32_t>(this->meshIndices.size());
    } else {
        shaderBufferComponents.verticeCount = static_cast<uint32_t>(this->meshVertices.size());
        shaderBufferComponents.indiceCount = -1;
    }
}

void ModelHandler::Model::populateShaderBufferComponents(std::vector<ModelHandler::ShadowVertexData> vertexData, VkCommandPool commandPool, VkQueue commandQueue, DeviceHandler::VulkanDevices vulkanDevices)
{
    Buffer::createDataBufferComponents(vertexData.data(), (sizeof(vertexData[0]) * vertexData.size()), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, commandPool, commandQueue, vulkanDevices, this->shaderBufferComponents.vertexBuffer, this->shaderBufferComponents.vertexBufferMemory);

    if (this->meshIndices.empty() == false) {
        Buffer::createDataBufferComponents(this->meshIndices.data(), (sizeof(this->meshIndices[0]) * this->meshIndices.size()), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, commandPool, commandQueue, vulkanDevices, this->shaderBufferComponents.indexBuffer, this->shaderBufferComponents.indexBufferMemory);
        
        shaderBufferComponents.verticeCount = -1;
        shaderBufferComponents.indiceCount = static_cast<uint32_t>(this->meshIndices.size());
    } else {
        shaderBufferComponents.verticeCount = static_cast<uint32_t>(this->meshVertices.size());
        shaderBufferComponents.indiceCount = -1;
    }
}

void ModelHandler::Model::cleanupModel(bool preserveTextureDetails, VkDevice vulkanLogicalDevice)
{
    vkDestroyBuffer(vulkanLogicalDevice, this->shaderBufferComponents.vertexBuffer, nullptr);
    vkFreeMemory(vulkanLogicalDevice, this->shaderBufferComponents.vertexBufferMemory, nullptr);

    if (this->meshIndices.empty() == false) {
    vkDestroyBuffer(vulkanLogicalDevice, this->shaderBufferComponents.indexBuffer, nullptr);
    vkFreeMemory(vulkanLogicalDevice, this->shaderBufferComponents.indexBufferMemory, nullptr);
    }

    if (preserveTextureDetails == false) {
        this->textureDetails.cleanupTextureDetails(vulkanLogicalDevice);
    }
}

void ModelHandler::populateVertexInputCreateInfo(std::vector<VkVertexInputAttributeDescription>& attributeDescriptions, VkVertexInputBindingDescription *bindingDescription, VkPipelineVertexInputStateCreateInfo& vertexInputCreateInfo)
{
    vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    
    vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputCreateInfo.pVertexBindingDescriptions = bindingDescription;
    vertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
}

void ModelHandler::populateInputAssemblyCreateInfo(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable, VkPipelineInputAssemblyStateCreateInfo& inputAssemblyCreateInfo)
{
    inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    
    inputAssemblyCreateInfo.topology = topology;  // form triangle primitives without vertex reuse.
    inputAssemblyCreateInfo.primitiveRestartEnable = primitiveRestartEnable;  // disable the possibility of vertex reuse.
}
