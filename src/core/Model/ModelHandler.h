#ifndef MODELHANDLER_H
#define MODELHANDLER_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <core/Shader/Image.h>
#include <core/VulkanInstance/DeviceHandler.h>

#include <vector>
#include <array>
#include <string>


namespace ModelHandler
{
    struct Vertex {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec2 UVCoordinates;
    };

    struct Model
    {
        std::string absoluteModelDirectory;  // the absolute directory of the model.
        
        std::vector<ModelHandler::Vertex> meshVertices;
        std::vector<uint32_t> meshIndices;

        // TODO: support for URI-encoded textures.
        std::string absoluteTextureImagePath;  // the absolute path of the texture image.
        Image::TextureDetails textureDetails;

        // vertex and index buffer personally created for the model.
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        // TODO: what do we do if the model doesn't support/have indices?
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        

        // load a glTF model from an absolute path.
        //
        // @param absoluteModelPath the absolute path of the model.
        void loadModelFromAbsolutePath(std::string absoluteModelPath);

        // create vertex and index buffers for this model.
        //
        // @param commandPool command pool to allocate necessary command buffers on.
        // @param commandQueue queue to submit necessary commands on.
        // @param vulkanDevices Vulkan logical and physical device to use in model buffers creation.
        void createModelBuffers(VkCommandPool commandPool, VkQueue commandQueue, DeviceHandler::VulkanDevices vulkanDevices);

        // cleanup the model.
        //
        // @param vulkanLogicalDevice Vulkan logical device to use in model cleanup.
        void cleanupModel(VkDevice vulkanLogicalDevice);
    };

    // TODO: remove.
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

    const std::vector<uint32_t> indices = {  // represents the index buffer.
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


#endif  // MODELHANDLER_H
