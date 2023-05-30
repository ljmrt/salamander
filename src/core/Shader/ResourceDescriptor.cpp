#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Shader/ResourceDescriptor.h>
#include <core/Model/ModelHandler.h>
#include <core/Logging/ErrorLogger.h>
#include <core/Defaults/Defaults.h>

#include <array>
#include <vector>


void ResourceDescriptor::populateBindingDescription(VkVertexInputBindingDescription& bindingDescription)
{
    bindingDescription.binding = 0;  // binding index in the "array" of bindings.
    bindingDescription.stride = sizeof(ModelHandler::Vertex);  // byte distance from one entry to another.
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}

void ResourceDescriptor::fetchAttributeDescriptions(std::array<VkVertexInputAttributeDescription, 3>& attributeDescriptions)
{
    // attribute formats use RGB for some reason.
    VkVertexInputAttributeDescription positionAttributeDescription{};
    
    positionAttributeDescription.binding = 0;
    positionAttributeDescription.location = 0;
    
    positionAttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
    positionAttributeDescription.offset = offsetof(ModelHandler::Vertex, position);


    VkVertexInputAttributeDescription normalAttributeDescription{};

    normalAttributeDescription.binding = 0;
    normalAttributeDescription.location = 1;
    
    normalAttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
    normalAttributeDescription.offset = offsetof(ModelHandler::Vertex, normal);

    
    VkVertexInputAttributeDescription UVCoordinatesAttributeDescription{};

    UVCoordinatesAttributeDescription.binding = 0;
    UVCoordinatesAttributeDescription.location = 2;
    
    UVCoordinatesAttributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
    UVCoordinatesAttributeDescription.offset = offsetof(ModelHandler::Vertex, UVCoordinates);


    attributeDescriptions = {positionAttributeDescription, normalAttributeDescription, UVCoordinatesAttributeDescription};
}

void ResourceDescriptor::populateDescriptorSetLayoutBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, VkDescriptorSetLayoutBinding& descriptorSetLayoutBinding)
{
    descriptorSetLayoutBinding.binding = binding;

    descriptorSetLayoutBinding.descriptorCount = 1;
    descriptorSetLayoutBinding.descriptorType = descriptorType;

    descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

    descriptorSetLayoutBinding.stageFlags = stageFlags;
}

void ResourceDescriptor::createDescriptorSetLayout(VkDevice vulkanLogicalDevice, VkDescriptorSetLayout& descriptorSetLayout)
{
    VkDescriptorSetLayoutBinding uniformBufferLayoutBinding{};
    ResourceDescriptor::populateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, (VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), uniformBufferLayoutBinding);

    VkDescriptorSetLayoutBinding combinedSamplerLayoutBinding{};
    ResourceDescriptor::populateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, combinedSamplerLayoutBinding);
    

    std::array<VkDescriptorSetLayoutBinding, 2> descriptorSetLayoutBindings = {uniformBufferLayoutBinding, combinedSamplerLayoutBinding};
    
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

void ResourceDescriptor::createDescriptorPool(VkDevice vulkanLogicalDevice, VkDescriptorPool& descriptorPool)
{
    VkDescriptorPoolSize uniformBufferPoolSize{};
    ResourceDescriptor::populateDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolSize combinedSamplerPoolSize{};
    ResourceDescriptor::populateDescriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT);
    
    std::array<VkDescriptorPoolSize, 2> descriptorPoolSizes = {uniformBufferPoolSize, combinedSamplerPoolSize};
    
    
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
    descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();

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

void ResourceDescriptor::populateDescriptorSets(std::vector<VkBuffer>& uniformBuffers, VkImageView textureImageView, VkSampler textureSampler, VkDevice vulkanLogicalDevice, std::vector<VkDescriptorSet>& descriptorSets)
{
    for (size_t i = 0; i < descriptorSets.size(); i += 1) {
        VkDescriptorBufferInfo descriptorBufferInfo{};

        descriptorBufferInfo.buffer = uniformBuffers[i];
        
        descriptorBufferInfo.offset = 0;
        descriptorBufferInfo.range = VK_WHOLE_SIZE;  // we're overwriting the entire buffer.


        VkDescriptorImageInfo descriptorImageInfo{};

        descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        descriptorImageInfo.imageView = textureImageView;

        descriptorImageInfo.sampler = textureSampler;


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


        std::array<VkWriteDescriptorSet, 2> writeDescriptorSets = {uniformBufferWriteDescriptorSet, combinedSamplerWriteDescriptorSet};

        vkUpdateDescriptorSets(vulkanLogicalDevice, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);  // populate/update the descriptor set.
    }
}
