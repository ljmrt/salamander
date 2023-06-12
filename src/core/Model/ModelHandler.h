#ifndef MODELHANDLER_H
#define MODELHANDLER_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <core/Shader/Image.h>
#include <core/VulkanInstance/DeviceHandler.h>

#include <vector>
#include <array>
#include <string>


namespace ModelHandler
{
    // different vertex data structs to populate the vertex buffer with, dependent on what pipeline is in use.
    struct SceneVertexData {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 UVCoordinates;
    };

    struct SceneNormalsVertexData {
        glm::vec3 position;
        glm::vec3 normal;
    };

    struct CubemapVertexData {
        glm::vec3 position;
    };

    struct ShaderBufferComponents {
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        uint32_t indiceCount;
    };

    struct Model
    {
        std::string absoluteModelDirectory;  // the absolute directory of the model.
        
        std::vector<ModelHandler::SceneVertexData> meshVertices;  // vertice compenets are normalized to a 0..1 range, expected to "reinterpet" this into the desired vertex data(see above structs).
        std::vector<uint32_t> meshIndices;

        glm::quat meshQuaternion = glm::identity<glm::quat>();  // the stored quaternion to rotate the mesh using.

        // TODO: support for URI-encoded textures.
        std::string absoluteTextureImagePath;  // the absolute path of the texture image.
        Image::TextureDetails textureDetails;

        // shader buffer components personally created for the model.
        // TODO: what do we do if the model doesn't support/have indices?
        ModelHandler::ShaderBufferComponents shaderBufferComponents;

        // load a glTF model from an absolute path.
        //
        // @param absoluteModelPath the absolute path of the model.
        void loadModelFromAbsolutePath(std::string absoluteModelPath);

        // normalize the mesh vertice normal values.
        void normalizeNormalValues();

        // populate the shader buffer components for this model.
        //
        // @param vertexData the vertex data to pass into the vertex buffer.
        // @param commandPool command pool to allocate necessary command buffers on.
        // @param commandQueue queue to submit necessary commands on.
        // @param vulkanDevices Vulkan logical and physical device to use in model buffers creation.
        void populateShaderBufferComponents(std::vector<ModelHandler::SceneVertexData> vertexData, VkCommandPool commandPool, VkQueue commandQueue, DeviceHandler::VulkanDevices vulkanDevices);
        
        // populate the shader buffer components for this model.
        //
        // @param vertexData the vertex data to pass into the vertex buffer.
        // @param commandPool command pool to allocate necessary command buffers on.
        // @param commandQueue queue to submit necessary commands on.
        // @param vulkanDevices Vulkan logical and physical device to use in model buffers creation.
        void populateShaderBufferComponents(std::vector<ModelHandler::SceneNormalsVertexData> vertexData, VkCommandPool commandPool, VkQueue commandQueue, DeviceHandler::VulkanDevices vulkanDevices);
        
        // populate the shader buffer components for this model.
        //
        // @param vertexData the vertex data to pass into the vertex buffer.
        // @param commandPool command pool to allocate necessary command buffers on.
        // @param commandQueue queue to submit necessary commands on.
        // @param vulkanDevices Vulkan logical and physical device to use in model buffers creation.
        void populateShaderBufferComponents(std::vector<ModelHandler::CubemapVertexData> vertexData, VkCommandPool commandPool, VkQueue commandQueue, DeviceHandler::VulkanDevices vulkanDevices);

        // cleanup the model.
        //
        // @param preserveTextureDetails if the model's texture details should be preserved(not destroyed).
        // @param vulkanLogicalDevice Vulkan logical device to use in model cleanup.
        void cleanupModel(bool preserveTextureDetails, VkDevice vulkanLogicalDevice);
    };


    // TODO: find a different method of preserving memory(allocate memory possibly).
    extern std::vector<VkVertexInputAttributeDescription> preservedCubemapAttributeDescriptions;  // preserved cubemap attribute descriptions(pointer reasons).
    extern VkVertexInputBindingDescription preservedCubemapBindingDescription;  // preserved cubemap binding description(pointer reasons).

    extern std::vector<VkVertexInputAttributeDescription> preservedSceneAttributeDescriptions;  // preserved scene attribute descriptions(pointer reasons).
    extern VkVertexInputBindingDescription preservedSceneBindingDescription;  // preserved scene binding description(pointer reasons).
    extern std::vector<VkVertexInputAttributeDescription> preservedSceneNormalsAttributeDescriptions;  // preserved scene normals attribute descriptions(pointer reasons).
    extern VkVertexInputBindingDescription preservedSceneNormalsBindingDescription;  // preserved scene normals binding description(pointer reasons).

    // populate a vertex input's create info.
    //
    // @param attributeDescriptions container of attribute descriptions.
    // @param bindingDescription pointer to the binding description to use in population.
    // @param vertexInputCreateInfo stored filled vertex input create info.
    void populateVertexInputCreateInfo(std::vector<VkVertexInputAttributeDescription>& attributeDescriptions, VkVertexInputBindingDescription *bindingDescription, VkPipelineVertexInputStateCreateInfo& vertexInputCreateInfo);

    // populate a input assembly's create info.
    //
    // @param topology see VkPipelineInputAssemblyStateCreateInfo documentation.
    // @param primitiveRestartEnable see VkPipelineInputAssemblyStateCreateInfo documentation.
    // @param inputAssemblyCreateInfo stored filled input assembly create info.
    void populateInputAssemblyCreateInfo(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable, VkPipelineInputAssemblyStateCreateInfo& inputAssemblyCreateInfo);
}


#endif  // MODELHANDLER_H
