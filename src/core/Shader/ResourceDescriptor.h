#ifndef RESOURCEDESCRIPTOR_H
#define RESOURCEDESCRIPTOR_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <array>
#include <vector>


namespace ResourceDescriptor
{
    // populate the passed binding description.
    //
    // @param stride see VkVertexInputBindingDescription documentation.
    // @param bindingDescription stored fetched binding description.
    void populateBindingDescription(uint32_t stride, VkVertexInputBindingDescription& bindingDescription);

    // populate a vertex input attribute description.
    //
    // @param location see VkVertexInputAttributeDescription documentation.
    // @param binding see VkVertexInputAttributeDescription documentation.
    // @param format see VkVertexInputAttributeDescription documentation.
    // @param offset see VkVertexInputAttributeDescription documentation.
    // @param vertexInputAttributeDescription populated vertex input attribute description.
    void populateVertexInputAttributeDescription(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset, VkVertexInputAttributeDescription& vertexInputAttributeDescription);

    // fetch the attribute descriptions used for the cubemap vertex attributes.
    //
    // @param attributeDescriptions fetched attribute descriptions.
    void fetchCubemapAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& attributeDescriptions);

    // fetch the attribute descriptions used for the scene vertex attributes.
    //
    // @param attributeDescriptions fetched attribute descriptions.
    void fetchSceneAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& attributeDescriptions);

    // fetch the attribute descriptions used for the scene normals vertex attributes.
    //
    // @param attributeDescriptions fetched attribute descriptions.
    void fetchSceneNormalsAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& attributeDescriptions);

    // populate a descriptor set layout binding.
    //
    // @param binding see VkDescriptorSetLayoutBinding documentation.
    // @param descriptorType see VkDescriptorSetLayoutBinding documentation.
    // @param stageFlags see VkDescriptorSetLayoutBinding documentation.
    // @param descriptorSetLayoutBinding populated descriptor set layout binding.
    void populateDescriptorSetLayoutBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, VkDescriptorSetLayoutBinding& descriptorSetLayoutBinding);
    
    // create descriptor set layout.
    //
    // @param descriptorSetLayoutBindings a container of descriptor set layout bindings.
    // @param vulkanLogicalDevice Vulkan logical device to use in descriptor set layout creation.
    // @palam descriptorSetLayout created descriptor set layout.
    void createDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding>& descriptorSetLayoutBindings, VkDevice vulkanLogicalDevice, VkDescriptorSetLayout& descriptorSetLayout);

    // populate a descriptor pool size.
    //
    // @param type see VkDescriptorPoolSize documentation.
    // @param descriptorCount see VkDescriptorPoolSize documentation.
    // @param descriptorPoolSize populated descriptor pool size.
    void populateDescriptorPoolSize(VkDescriptorType type, uint32_t descriptorCount, VkDescriptorPoolSize& descriptorPoolSize);

    // create descriptor pool.
    //
    // @param combinedSamplerCount the amount of combined samplers that will be allocated in the pool.
    // @param vulkanLogicalDevice Vulkan logical device to use in descriptor pool creation.
    // @param descriptorPool created descriptor pool.
    void createDescriptorPool(uint32_t combinedSamplerCount, VkDevice vulkanLogicalDevice, VkDescriptorPool& descriptorPool);

    // create descriptor sets.
    //
    // @param descriptorSetLayout descriptor set layout to use in descriptor sets creation.
    // @param descriptorPool descriptor pool to use in descriptor sets creation.
    // @param vulkanLogicalDevice Vulkan logical device to use in descriptor sets creation.
    // @param descriptorSets created descriptorSets.
    void createDescriptorSets(VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool, VkDevice vulkanLogicalDevice, std::vector<VkDescriptorSet>& descriptorSets);

    // populate a write descriptor set struct.
    //
    // @param dstSet see VkWriteDescriptorSet documentation.
    // @param dstBinding see VkWriteDescriptorSet documentation.
    // @param descriptorType see VkWriteDescriptorSet documentation.
    // @param imageInfo see VkWriteDescriptorSet documentation.
    // @param bufferInfo see VkWriteDescriptorSet documentation.
    // @param writeDescriptorSet populated write descriptor set.
    void populateWriteDescriptorSet(VkDescriptorSet dstSet, uint32_t dstBinding, VkDescriptorType descriptorType, const VkDescriptorImageInfo *imageInfo, const VkDescriptorBufferInfo *bufferInfo, VkWriteDescriptorSet& writeDescriptorSet);

    // populate a descriptor buffer info struct.
    //
    // @param buffer see VkDescriptorBufferInfo documentation.
    // @param offset see VkDescriptorBufferInfo documentation.
    // @param range see VkDescriptorBufferInfo documentation.
    // @param descriptorBufferInfo populated descriptor buffer info.
    void populateDescriptorBufferInfo(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range, VkDescriptorBufferInfo& descriptorBufferInfo);

    // populate a descriptor image info struct.
    //
    // @param sampler see VkDescriptorImageInfo documentation.
    // @param imageView see VkDescriptorImageInfo documentation.
    // @param imageLayout see VkDescriptorImageInfo documentation.
    // @param descriptorImageInfo populated descriptor image info.
    void populateDescriptorImageInfo(VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout, VkDescriptorImageInfo& descriptorImageInfo);
    
    // populate the supplied descriptor sets.
    // by default, a uniform buffer descriptor set with a binding of 0 is provided.
    // all provided write descriptor sets will have their dstSet accordingly.
    //
    // @param uniformBuffers uniform buffers to populate the descriptor sets with.
    // @param additionalWriteDescriptorSets additional write descriptor sets in addition to the default uniform buffer write descriptor set.
    // @param vulkanLogicalDevice Vulkan logical device to use in descriptor sets population.
    // @param descriptorSets populated descriptor sets.
    void populateDescriptorSets(std::vector<VkBuffer>& uniformBuffers, std::vector<VkWriteDescriptorSet>& additionalWriteDescriptorSets, VkDevice vulkanLogicalDevice, std::vector<VkDescriptorSet>& descriptorSets);
}


#endif  // RESOURCEDESCRIPTOR_H
