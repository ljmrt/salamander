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
    // @param bindingDescription stored fetched binding description.
    void populateBindingDescription(VkVertexInputBindingDescription& bindingDescription);

    // fetch the attribute descriptions used for the vertices.
    //
    // @param attributeDescriptions stored fetched attribute descriptions.
    void fetchAttributeDescriptions(std::array<VkVertexInputAttributeDescription, 3>& attributeDescriptions);
    
    // create descriptor set layout.
    //
    // @param vulkanLogicalDevice Vulkan logical device to use in descriptor set layout creation.
    // @palam descriptorSetLayout created descriptor set layout.
    void createDescriptorSetLayout(VkDevice vulkanLogicalDevice, VkDescriptorSetLayout& descriptorSetLayout);

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
    // @param textureSampler the main texture's sampler to use in descriptor sets population.
    // @param vulkanLogicalDevice Vulkan logical device to use in descriptor sets population.
    // @param descriptorSets populated descriptor sets.
    void populateDescriptorSets(std::vector<VkBuffer>& uniformBuffers, VkImageView textureImageView, VkSampler textureSampler, VkDevice vulkanLogicalDevice, std::vector<VkDescriptorSet>& descriptorSets);
}


#endif  // RESOURCEDESCRIPTOR_H
