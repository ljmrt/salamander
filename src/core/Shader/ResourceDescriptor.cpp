#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Shader/ResourceDescriptor.h>
#include <core/Model/VertexHandler.h>
#include <core/Logging/ErrorLogger.h>
#include <core/Defaults/Defaults.h>

#include <vector>


void ResourceDescriptor::populateBindingDescription(VkVertexInputBindingDescription& bindingDescription)
{
    bindingDescription.binding = 0;  // binding index in the "array" of bindings.
    bindingDescription.stride = sizeof(VertexHandler::Vertex);  // byte distance from one entry to another.
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}

void ResourceDescriptor::fetchAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& attributeDescriptions)
{
    VkVertexInputAttributeDescription attributeDescription{};
    
    // position description.
    attributeDescription.binding = 0;
    attributeDescription.location = 0;  // "location" keyword in shader.
    attributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescription.offset = offsetof(VertexHandler::Vertex, position);

    attributeDescriptions.push_back(attributeDescription);

    // color description.
    attributeDescription.binding = 0;
    attributeDescription.location = 1;  // "location" keyword in shader.
    attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescription.offset = offsetof(VertexHandler::Vertex, color);

    attributeDescriptions.push_back(attributeDescription);
}

void ResourceDescriptor::createDescriptorSetLayout(VkDevice vulkanLogicalDevice, VkDescriptorSetLayout& descriptorSetLayout)
{
    VkDescriptorSetLayoutBinding uniformBufferLayoutBinding{};
    
    uniformBufferLayoutBinding.binding = 0;  // specified in the vertex shader.
    
    uniformBufferLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformBufferLayoutBinding.descriptorCount = 1;

    uniformBufferLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;  // descriptor only referenced in the vertex shader.

    uniformBufferLayoutBinding.pImmutableSamplers = nullptr;


    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    
    descriptorSetLayoutCreateInfo.bindingCount = 1;
    descriptorSetLayoutCreateInfo.pBindings = &uniformBufferLayoutBinding;

    VkResult descriptorSetLayoutCreationResult = vkCreateDescriptorSetLayout(vulkanLogicalDevice, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);
    if (descriptorSetLayoutCreationResult != VK_SUCCESS) {
        throwDebugException("Failed to create descriptor set layout.");
    }
}

void ResourceDescriptor::createDescriptorPool(VkDevice vulkanLogicalDevice, VkDescriptorPool& descriptorPool)
{
    VkDescriptorPoolSize descriptorPoolSize{};
    descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    
    descriptorPoolSize.descriptorCount = static_cast<uint32_t>(Defaults::rendererDefaults.MAX_FRAMES_IN_FLIGHT);


    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

    descriptorPoolCreateInfo.poolSizeCount = 1;
    descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;

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

void ResourceDescriptor::populateDescriptorSets(std::vector<VkBuffer>& uniformBuffers, VkDevice vulkanLogicalDevice, std::vector<VkDescriptorSet>& descriptorSets)
{
    for (size_t i = 0; i < descriptorSets.size(); i += 1) {
        VkDescriptorBufferInfo descriptorBufferInfo{};

        descriptorBufferInfo.buffer = uniformBuffers[i];
        
        descriptorBufferInfo.offset = 0;
        descriptorBufferInfo.range = VK_WHOLE_SIZE;  // we're overwriting the entire buffer.


        VkWriteDescriptorSet writeDescriptorSet{};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

        writeDescriptorSet.dstSet = descriptorSets[i];
        writeDescriptorSet.dstBinding = 0;  // the uniform buffer binding index.
        writeDescriptorSet.dstArrayElement = 0;  // we're not using an array.

        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;  // we're using this for our uniform buffer.
        writeDescriptorSet.descriptorCount = 1;

        writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;
        writeDescriptorSet.pImageInfo = nullptr;
        writeDescriptorSet.pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(vulkanLogicalDevice, 1, &writeDescriptorSet, 0, nullptr);  // populate/update the descriptor set.
    }
}
