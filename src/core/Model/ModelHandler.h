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
    struct Vertex {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec2 UVCoordinates;
    };

    struct Model
    {
        std::string absoluteModelDirectory;  // the absolute directory of the model.
        
        std::vector<ModelHandler::Vertex> meshVertices;  // vertice compenets are normalized to a 0..1 range.
        std::vector<uint32_t> meshIndices;

        glm::quat meshQuaternion = glm::identity<glm::quat>();  // the stored quaternion to rotate the mesh using.

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


    extern std::array<VkVertexInputAttributeDescription, 3> preservedAttributeDescriptions;  // preserved attribute descriptions(pointer reasons).
    extern VkVertexInputBindingDescription preservedBindingDescription;  // preserved binding description(pointer reasons).

    // TODO: move this into utils or a math namespace.
    // normalize value to targetMinimumValue..targetMaximumValue through the equation shown here(https://stats.stackexchange.com/questions/281162/scale-a-number-between-a-range).
    //
    // @param initialValue the initial value.
    // @param initialRangeMinimumValue the lowest(minimum) value in the initial range.
    // @param initialRangeMaximumValue the highest(maximum) value in the initial range.
    // @param targetRangeMinimumValue the lowest(minimum) value in the target range.
    // @param targetRangeMaximumValue the highest(maximum) value in the target range.
    // @return normalized value.
    float normalizeValueToRanges(float initialValue, float initialRangeMinimumValue, float initialRangeMaximumValue, float targetRangeMinimumValue, float targetRangeMaximumValue);
    
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
