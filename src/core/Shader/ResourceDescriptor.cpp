#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Shader/ResourceDescriptor.h>
#include <core/Model/ModelHandler.h>
#include <core/Logging/ErrorLogger.h>
#include <core/Defaults/Defaults.h>

#include <array>
#include <vector>


void ResourceDescriptor::populateBindingDescription(uint32_t stride, VkVertexInputBindingDescription& bindingDescription)
{
    bindingDescription.binding = 0;  // binding index to fetch vertex attribute information on.
    bindingDescription.stride = stride;
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}

void ResourceDescriptor::populateVertexInputAttributeDescription(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset, VkVertexInputAttributeDescription& vertexInputAttributeDescription)
{
    vertexInputAttributeDescription.location = location;
    vertexInputAttributeDescription.binding = binding;
    
    vertexInputAttributeDescription.format = format;
    vertexInputAttributeDescription.offset = offset;
}

void ResourceDescriptor::fetchCubemapAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& attributeDescriptions)
{
    VkVertexInputAttributeDescription positionAttributeDescription{};
    ResourceDescriptor::populateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(ModelHandler::CubemapVertexData, position), positionAttributeDescription);
    
    attributeDescriptions = {positionAttributeDescription};
}

void ResourceDescriptor::fetchSceneAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& attributeDescriptions)
{
    VkVertexInputAttributeDescription positionAttributeDescription{};
    ResourceDescriptor::populateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(ModelHandler::SceneVertexData, position), positionAttributeDescription);

    VkVertexInputAttributeDescription normalAttributeDescription{};
    ResourceDescriptor::populateVertexInputAttributeDescription(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(ModelHandler::SceneVertexData, normal), normalAttributeDescription);
    
    VkVertexInputAttributeDescription UVCoordinatesAttributeDescription{};
    ResourceDescriptor::populateVertexInputAttributeDescription(2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(ModelHandler::SceneVertexData, UVCoordinates), UVCoordinatesAttributeDescription);

    attributeDescriptions = {positionAttributeDescription, normalAttributeDescription, UVCoordinatesAttributeDescription};
}

void ResourceDescriptor::fetchSceneNormalsAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& attributeDescriptions)
{
    VkVertexInputAttributeDescription positionAttributeDescription{};
    ResourceDescriptor::populateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(ModelHandler::SceneNormalsVertexData, position), positionAttributeDescription);

    VkVertexInputAttributeDescription normalAttributeDescription{};
    ResourceDescriptor::populateVertexInputAttributeDescription(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(ModelHandler::SceneNormalsVertexData, normal), normalAttributeDescription);
    
    attributeDescriptions = {positionAttributeDescription, normalAttributeDescription};
}

void ResourceDescriptor::populateDescriptorSetLayoutBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, VkDescriptorSetLayoutBinding& descriptorSetLayoutBinding)
{
    descriptorSetLayoutBinding.binding = binding;

    descriptorSetLayoutBinding.descriptorCount = 1;
    descriptorSetLayoutBinding.descriptorType = descriptorType;

    descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

    descriptorSetLayoutBinding.stageFlags = stageFlags;
}

void ResourceDescriptor::createDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding>& descriptorSetLayoutBindings, VkDevice vulkanLogicalDevice, VkDescriptorSetLayout& descriptorSetLayout)
{
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    
    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size());
    descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings.data();

    VkResult descriptorSetLayoutCreationResult = vkCreateDescriptorSetLayout(vulkanLogicalDevice, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);
    if (descriptorSetLayoutCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create descriptor set layout.");
    }
}

void ResourceDescriptor::populateDescriptorPoolSize(VkDescriptorType type, uint32_t descriptorCount, VkDescriptorPoolSize& descriptorPoolSize)
{
    descriptorPoolSize.type = type;
    descriptorPoolSize.descriptorCount = descriptorCount;
}

void ResourceDescriptor::createDescriptorPool(bool useCombinedSampler, VkDevice vulkanLogicalDevice, VkDescriptorPool& descriptorPool)
{
    VkDescriptorPoolSize descriptorPoolSizes[2];
    uint32_t descriptorPoolSizeCount = -1;
    
    VkDescriptorPoolSize uniformBufferPoolSize{};
    ResourceDescriptor::populateDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT, uniformBufferPoolSize);

    descriptorPoolSizes[0] = uniformBufferPoolSize;
    descriptorPoolSizeCount = 1;
    if (useCombinedSampler == true) {
        VkDescriptorPoolSize combinedSamplerPoolSize{};
        ResourceDescriptor::populateDescriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT, combinedSamplerPoolSize);

        descriptorPoolSizes[1] = combinedSamplerPoolSize;
        descriptorPoolSizeCount = 2;
    }
    
    
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

    descriptorPoolCreateInfo.poolSizeCount = descriptorPoolSizeCount;
    descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes;

    descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT);

    VkResult descriptorPoolCreationResult = vkCreateDescriptorPool(vulkanLogicalDevice, &descriptorPoolCreateInfo, nullptr, &descriptorPool);
    if (descriptorPoolCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create descriptor pool.");
    }
}

void ResourceDescriptor::createDescriptorSets(VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool, VkDevice vulkanLogicalDevice, std::vector<VkDescriptorSet>& descriptorSets)
{
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts(Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
    
    VkDescriptorSetAllocateInfo descriptorSetsAllocateInfo{};
    descriptorSetsAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

    descriptorSetsAllocateInfo.descriptorPool = descriptorPool;
    
    descriptorSetsAllocateInfo.descriptorSetCount = static_cast<uint32_t>(Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT);
    descriptorSetsAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

    descriptorSets.resize(Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT);
    VkResult descriptorSetsAllocationResult = vkAllocateDescriptorSets(vulkanLogicalDevice, &descriptorSetsAllocateInfo, descriptorSets.data());
    if (descriptorSetsAllocationResult != VK_SUCCESS) {
        throwDebugException("Failed to allocate descriptor sets.");
    }
}

void ResourceDescriptor::populateDescriptorSets(std::vector<VkBuffer>& uniformBuffers, VkImageView textureImageView, VkSampler combinedSampler, bool combinedSamplerProvided, VkDevice vulkanLogicalDevice, std::vector<VkDescriptorSet>& descriptorSets)
{
    for (size_t i = 0; i < descriptorSets.size(); i += 1) {
        VkDescriptorBufferInfo descriptorBufferInfo{};

        descriptorBufferInfo.buffer = uniformBuffers[i];
        
        descriptorBufferInfo.offset = 0;
        descriptorBufferInfo.range = VK_WHOLE_SIZE;  // we're overwriting the entire buffer.


        VkDescriptorImageInfo descriptorImageInfo{};

        descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        descriptorImageInfo.imageView = textureImageView;

        if (combinedSamplerProvided == true) {
            descriptorImageInfo.sampler = combinedSampler;
        }


        VkWriteDescriptorSet uniformBufferWriteDescriptorSet{};
        uniformBufferWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

        uniformBufferWriteDescriptorSet.dstSet = descriptorSets[i];
        uniformBufferWriteDescriptorSet.dstBinding = 0;  // the uniform buffer binding index.
        uniformBufferWriteDescriptorSet.dstArrayElement = 0;

        uniformBufferWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformBufferWriteDescriptorSet.descriptorCount = 1;

        uniformBufferWriteDescriptorSet.pBufferInfo = &descriptorBufferInfo;
        uniformBufferWriteDescriptorSet.pImageInfo = nullptr;
        uniformBufferWriteDescriptorSet.pTexelBufferView = nullptr;

        VkWriteDescriptorSet writeDescriptorSets[2];  // bad hack, but allocate the highest used size.
        uint32_t writeDescriptorSetsSize = -1;

        if (combinedSamplerProvided == true) {
            VkWriteDescriptorSet combinedSamplerWriteDescriptorSet{};
            combinedSamplerWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

            combinedSamplerWriteDescriptorSet.dstSet = descriptorSets[i];
            combinedSamplerWriteDescriptorSet.dstBinding = 1;  // the combined image sampler binding index.
            combinedSamplerWriteDescriptorSet.dstArrayElement = 0;

            combinedSamplerWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            combinedSamplerWriteDescriptorSet.descriptorCount = 1;

            combinedSamplerWriteDescriptorSet.pBufferInfo = nullptr;
            combinedSamplerWriteDescriptorSet.pImageInfo = &descriptorImageInfo;
            combinedSamplerWriteDescriptorSet.pTexelBufferView = nullptr;

            writeDescriptorSets[0] = uniformBufferWriteDescriptorSet;
            writeDescriptorSets[1] = combinedSamplerWriteDescriptorSet;
            writeDescriptorSetsSize = 2;
        } else {
            writeDescriptorSets[0] = uniformBufferWriteDescriptorSet;
            writeDescriptorSetsSize = 1;
        }

        vkUpdateDescriptorSets(vulkanLogicalDevice, writeDescriptorSetsSize, writeDescriptorSets, 0, nullptr);  // populate/update the descriptor set.
    }
}
