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

    VkVertexInputAttributeDescription tangentAttributeDescription{};
    ResourceDescriptor::populateVertexInputAttributeDescription(2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(ModelHandler::SceneVertexData, tangent), tangentAttributeDescription);
    
    VkVertexInputAttributeDescription UVCoordinatesAttributeDescription{};
    ResourceDescriptor::populateVertexInputAttributeDescription(3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(ModelHandler::SceneVertexData, UVCoordinates), UVCoordinatesAttributeDescription);

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

void ResourceDescriptor::fetchShadowAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& attributeDescriptions)
{
    // currently the same as the cubemap attribute descriptions, but want to avoid a direct call.
    VkVertexInputAttributeDescription positionAttributeDescription{};
    ResourceDescriptor::populateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(ModelHandler::CubemapVertexData, position), positionAttributeDescription);
    
    attributeDescriptions = {positionAttributeDescription};
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

void ResourceDescriptor::createDescriptorPool(uint32_t combinedSamplerCount, VkDevice vulkanLogicalDevice, VkDescriptorPool& descriptorPool)
{
    VkDescriptorPoolSize descriptorPoolSizes[2];
    uint32_t descriptorPoolSizeCount = -1;
    
    VkDescriptorPoolSize uniformBufferPoolSize{};
    ResourceDescriptor::populateDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT, uniformBufferPoolSize);

    descriptorPoolSizes[0] = uniformBufferPoolSize;
    descriptorPoolSizeCount = 1;
    if (combinedSamplerCount > 0) {
        VkDescriptorPoolSize combinedSamplerPoolSize{};
        ResourceDescriptor::populateDescriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, (Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT * combinedSamplerCount), combinedSamplerPoolSize);

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

void ResourceDescriptor::populateWriteDescriptorSet(VkDescriptorSet dstSet, uint32_t dstBinding, VkDescriptorType descriptorType, const VkDescriptorImageInfo *imageInfo, const VkDescriptorBufferInfo *bufferInfo, VkWriteDescriptorSet& writeDescriptorSet)
{
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

    writeDescriptorSet.dstSet = dstSet;
    writeDescriptorSet.dstBinding = dstBinding;
    writeDescriptorSet.dstArrayElement = 0;

    writeDescriptorSet.descriptorType = descriptorType;
    writeDescriptorSet.descriptorCount = 1;

    writeDescriptorSet.pImageInfo = imageInfo;
    writeDescriptorSet.pBufferInfo = bufferInfo;
    writeDescriptorSet.pTexelBufferView = nullptr;
}

void ResourceDescriptor::populateDescriptorBufferInfo(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range, VkDescriptorBufferInfo& descriptorBufferInfo)
{
    descriptorBufferInfo.buffer = buffer;
    descriptorBufferInfo.offset = offset;
    descriptorBufferInfo.range = range;
}

void ResourceDescriptor::populateDescriptorImageInfo(VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout, VkDescriptorImageInfo& descriptorImageInfo)
{
    descriptorImageInfo.sampler = sampler;
    descriptorImageInfo.imageView = imageView;
    descriptorImageInfo.imageLayout = imageLayout;
}

void ResourceDescriptor::populateDescriptorSets(std::vector<VkBuffer>& uniformBuffers, std::vector<VkWriteDescriptorSet>& additionalWriteDescriptorSets, VkDevice vulkanLogicalDevice, std::vector<VkDescriptorSet>& descriptorSets)
{
    for (size_t i = 0; i < descriptorSets.size(); i += 1) {
        std::vector<VkWriteDescriptorSet> writeDescriptorSets;
        

        VkDescriptorBufferInfo descriptorBufferInfo{};

        descriptorBufferInfo.buffer = uniformBuffers[i];
        
        descriptorBufferInfo.offset = 0;
        descriptorBufferInfo.range = VK_WHOLE_SIZE;  // we're overwriting the entire buffer.

        VkWriteDescriptorSet uniformBufferWriteDescriptorSet{};
        ResourceDescriptor::populateWriteDescriptorSet(descriptorSets[i], 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nullptr, &descriptorBufferInfo, uniformBufferWriteDescriptorSet);
        writeDescriptorSets.push_back(uniformBufferWriteDescriptorSet);

        
        for (VkWriteDescriptorSet additionalWriteDescriptorSet : additionalWriteDescriptorSets) {
            additionalWriteDescriptorSet.dstSet = descriptorSets[i];
            writeDescriptorSets.push_back(additionalWriteDescriptorSet);
        }
        

        vkUpdateDescriptorSets(vulkanLogicalDevice, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);  // populate/update the descriptor set.
    }
}
