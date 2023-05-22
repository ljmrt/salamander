#ifndef RENDERER_H
#define RENDERER_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/VulkanInstance.h>
#include <core/Shader/Shader.h>
#include <core/Model/ModelHandler.h>

#include <vector>


class Renderer
{
private:
    VkDevice *m_vulkanLogicalDevice;  // pointer to this application's Vulkan instance.
    
    VkRenderPass m_renderPass;  // this graphics pipeline's render pass.
    Shader::PipelineShaders m_pipelineShaders;  // graphics pipeline shader stages.
    
    VkDescriptorSetLayout m_descriptorSetLayout;  // this pipeline layout's descriptor set layout.
    VkDescriptorPool m_descriptorPool;  // the descriptor pool to use in descriptor set creation.
    std::vector<VkDescriptorSet> m_descriptorSets;  // uniform buffer descriptor sets.
    
    VkPipelineLayout m_pipelineLayout;  // this graphics pipeline's pipeline layout.
    VkPipeline m_graphicsPipeline;  // graphics pipeline

    // TODO: move these to vulkan display details.
    VkCommandPool m_graphicsCommandPool;  // a command pool used for graphics command buffers.
    std::vector<VkCommandBuffer> m_graphicsCommandBuffers;  // child command buffers under the graphics command pool.

    std::vector<VkBuffer> m_uniformBuffers;  // the shader uniform buffers.
    std::vector<VkDeviceMemory> m_uniformBuffersMemory;  // the uniform buffers' memory.
    std::vector<void *> m_mappedUniformBuffersMemory;  // the mapped memory of the uniform buffers.

    std::vector<VkSemaphore> m_imageAvailibleSemaphores;  // semaphore used to make the GPU wait to continue until the next availible image index in the swapchain has been fetched.
    std::vector<VkSemaphore> m_renderFinishedSemaphores;  // semaphore used to make the GPU wait to continue until the current frame has finished rendering.
    std::vector<VkFence> m_inFlightFences;  // fence used to synchronize the GPU and CPU before begining to draw another frame.]

    size_t m_currentFrame = 0;  // the current "frame" in context of the "in flight" frames.

    ModelHandler::Model m_mainModel;  // the main loaded model.
    

    // populate a color attachment description and reference.
    //
    // @param swapchainImageFormat the image format of the swapchain, used in populating the color attachment components.
    // @param msaaSampleCount the amount of msaa samples.
    // @param colorAttachmentDescription populated color attachment description.
    // @param colorAttachmentReference populated color attachment reference.
    // @param colorAttachmentResolveDescription populated color attachment resolve description.
    // @param colorAttachmentResolveReference populated color attachment resolve reference.
    void populateColorAttachmentComponents(VkFormat swapchainImageFormat, VkSampleCountFlagBits msaaSampleCount, VkAttachmentDescription& colorAttachmentDescription, VkAttachmentReference& colorAttachmentReference, VkAttachmentDescription& colorAttachmentResolveDescription, VkAttachmentReference& colorAttachmentResolveReference);

    // populate a depth attachment description and reference.
    //
    // @param msaaSampleCount the amount of msaa samples.
    // @param vulkanPhysicalDevice Vulkan physical device to use in depth attachment population.
    // @param depthAttachmentDescription populated depth attachment description.
    // @param depthAttachmentReference populated depth attachment reference.
    void populateDepthAttachmentComponents(VkSampleCountFlagBits msaaSampleCount, VkPhysicalDevice vulkanPhysicalDevice, VkAttachmentDescription& depthAttachmentDescription, VkAttachmentReference& depthAttachmentReference);
    
    // populate a subpass's description.
    //
    // @param colorAttachmentReference color attachment reference to use in subpass description.
    // @param colorAttachmentResolveReference color attachment resolve reference to use in subpass description.
    // @param depthAttachmentReference depth attachment reference to use in subpass description.
    // @param subpassDescription populated subpass description.
    void populateSubpassDescription(VkAttachmentReference& colorAttachmentReference, VkAttachmentReference& colorAttachmentResolveReference, VkAttachmentReference& depthAttachmentReference, VkSubpassDescription& subpassDescription);
    
    // create member render pass.
    //
    // @param swapchainImageFormat the swapchain image format to use in member render pass creation.
    // @param msaaSampleCount the amount of msaa samples.
    // @param vulkanPhysicalDevice Vulkan physical device to use in member render pass creation.
    void createMemberRenderPass(VkFormat swapchainImageFormat, VkSampleCountFlagBits msaaSampleCount, VkPhysicalDevice vulkanPhysicalDevice);

    // populate a viewport's create info.
    //
    // @param viewportCreateInfo populated viewport create info.
    void populateViewportCreateInfo(VkPipelineViewportStateCreateInfo& viewportCreateInfo);

    // populate a rasterization's create info.
    //
    // @param rasterizationCreateInfo populated rasterization create info.
    void populateRasterizationCreateInfo(VkPipelineRasterizationStateCreateInfo& rasterizationCreateInfo);

    // fetch the maximum usable sample count for a physical device.
    //
    // @param vulkanPhysicalDevice Vulkan physical device to use in maximum usable sample count fetch.
    // @param maximumUsableSampleCount the (fetched) maximum usable sample count.
    void fetchMaximumUsableSampleCount(VkPhysicalDevice vulkanPhysicalDevice, VkSampleCountFlagBits& maximumUsableSampleCount);

    // populate a multisampling's create info.
    //
    // @param msaaSampleCount the amount of msaa samples.
    // @param multisamplingCreateInfo populated multisampling create info.
    void populateMultisamplingCreateInfo(VkSampleCountFlagBits msaaSampleCount, VkPipelineMultisampleStateCreateInfo& multisamplingCreateInfo);

    // populate a depth stencil's create info.
    //
    // @param depthStencilCreateInfo populated depth stencil create info.
    void populateDepthStencilCreateInfo(VkPipelineDepthStencilStateCreateInfo& depthStencilCreateInfo);

    // populate color blend components(color attachment and color blend's create info).
    //
    // @param colorblendAttachment populated color blend attachment.
    // @param colorBlendCreateInfo populated color blend create info.
    void populateColorBlendComponents(VkPipelineColorBlendAttachmentState& colorBlendAttachment, VkPipelineColorBlendStateCreateInfo& colorBlendCreateInfo);

    // populate a dynamic states's create info.
    //
    // @param dynamicStates dynamic states to use in populating.
    // @param dynamicStatesCreateInfo populated dynamic states create info.
    void populateDynamicStatesCreateInfo(std::vector<VkDynamicState>& dynamicStates, VkPipelineDynamicStateCreateInfo& dynamicStatesCreateInfo);

    // create a pipeline layout.
    void createMemberPipelineLayout();
    
    // create member Vulkan graphics pipeline.
    //
    // @param msaaSampleCount the amount of msaa samples.
    void createMemberGraphicsPipeline(VkSampleCountFlagBits msaaSampleCount);

    // create member synchronization objects(semaphores, fences).
    void createMemberSynchronizationObjects();

    // draw a frame onto the screen using all of the configured render passes, pipelines, etc.
    //
    // @param currentFrame current swapchain frame to draw.
    // @param displayDetails display details to use in frame drawing and possible swapchain recreation.
    // @param vulkanPhysicalDevice the Vulkan instance's physical device to use in possible swapchain recreation.
    // @param graphicsQueue graphics queue to queue commands to.
    // @param presentationQueue presentation queue to queue commands to.
    void drawFrame(DisplayManager::DisplayDetails& displayDetails, VkPhysicalDevice vulkanPhysicalDevice, VkQueue graphicsQueue, VkQueue presentationQueue);
public:
    // set the pointer to the Vulkan instance's logical device.
    //
    // @param vulkanLogicalDevice pointer to set personal pointer to.
    void setVulkanLogicalDevice(VkDevice *vulkanLogicalDevice);
    
    // render/main loop.
    //
    // @param displayDetails the display details to use in rendering.
    // @param graphicsFamilyIndex index of the graphics queue family.
    // @param vulkanPhysicalDevice the Vulkan instance's physical device to use in frame drawing.
    void render(DisplayManager::DisplayDetails& displayDetails, size_t graphicsFamilyIndex, VkPhysicalDevice vulkanPhysicalDevice);

    // terminates/destroys renderer and its members.
    void cleanupRenderer();

    // Renderer constructor.
    Renderer();
};


#endif  // RENDERER_H
