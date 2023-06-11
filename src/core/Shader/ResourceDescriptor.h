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
    // @param vulkanLogicalDevice Vulkan logical device to use in descriptor pool creation.
    // @param descriptorPool created descriptor pool.
    void createDescriptorPool(VkDevice vulkanLogicalDevice, VkDescriptorPool& descriptorPool);

    // create descriptor sets.
    //
    // @param descriptorSetLayout descriptor set layout to use in descriptor sets creation.
    // @param descriptorPool descriptor pool to use in descriptor sets creation.
    // @param vulkanLogicalDevice Vulkan logical device to use in descriptor sets creation.
    // @param descriptorSets created descriptorSets.
    void createDescriptorSets(VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool, VkDevice vulkanLogicalDevice, std::vector<VkDescriptorSet>& descriptorSets);
    
    // populate descriptor sets.
    //
    // @param uniformBuffers uniform buffers to populate the descriptor sets with.
    // @param textureImageView the main texture's image view to use in descriptor sets population.
    // @param combinedSampler the main texture's sampler to use in descriptor sets population.
    // @param combinedSamplerProvided if a combined sampler is provided.
    // @param vulkanLogicalDevice Vulkan logical device to use in descriptor sets population.
    // @param descriptorSets populated descriptor sets.
    void populateDescriptorSets(std::vector<VkBuffer>& uniformBuffers, VkImageView textureImageView, VkSampler combinedSampler, bool combinedSamplerProvided, VkDevice vulkanLogicalDevice, std::vector<VkDescriptorSet>& descriptorSets);
}


#endif  // RESOURCEDESCRIPTOR_H
