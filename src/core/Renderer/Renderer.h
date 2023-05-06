#ifndef RENDERER_H
#define RENDERER_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/VulkanInstance.h>
#include <core/Shader/Shader.h>

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

    VkBuffer m_vertexBuffer;  // vertex buffer.
    VkDeviceMemory m_vertexBufferMemory;  // the vertex buffer's memory.
    VkBuffer m_indexBuffer;  // index buffer.
    VkDeviceMemory m_indexBufferMemory;  // the index buffer's memory.

    std::vector<VkBuffer> m_uniformBuffers;  // the shader uniform buffers.
    std::vector<VkDeviceMemory> m_uniformBuffersMemory;  // the uniform buffers' memory.
    std::vector<void *> m_mappedUniformBuffersMemory;  // the mapped memory of the uniform buffers.

    VkImage m_textureImage;  // the main texture image.
    const std::string m_textureImageFilename = "gnulogo.png";  // the main texture image's filename(under "assets/textures").
    VkDeviceMemory m_textureImageMemory;  // the main texture image's memory.

    std::vector<VkSemaphore> m_imageAvailibleSemaphores;  // semaphore used to make the GPU wait to continue until the next availible image index in the swapchain has been fetched.
    std::vector<VkSemaphore> m_renderFinishedSemaphores;  // semaphore used to make the GPU wait to continue until the current frame has finished rendering.
    std::vector<VkFence> m_inFlightFences;  // fence used to synchronize the GPU and CPU before begining to draw another frame.]

    size_t m_currentFrame = 0;  // the current "frame" in context of the "in flight" frames.
    

    // populate a color attachment description and reference.
    //
    // @param swapchainImageFormat the image format of the swapchain, used in populating the color attachment components.
    // @param colorAttachmentDescription stored populated color attachment description.
    // @param colorAttachmentReference stored populated color attachment reference.
    void populateColorAttachmentComponents(VkFormat swapchainImageFormat, VkAttachmentDescription& colorAttachmentDescription, VkAttachmentReference& colorAttachmentReference);
    
    // populate a subpass's description.
    //
    // @param colorAttachmentReference color attachment reference to use in subpass description.
    // @param subpassDescription stored populated subpass description.
    void populateSubpassDescription(VkAttachmentReference *colorAttachmentReference, VkSubpassDescription& subpassDescription);
    
    // create member render pass.
    //
    // @param swapchainImageFormat the swapchain image format to use in render pass creation.
    void createMemberRenderPass(VkFormat swapchainImageFormat);

    // populate a viewport's create info.
    //
    // @param viewportCreateInfo stored populated viewport create info.
    void populateViewportCreateInfo(VkPipelineViewportStateCreateInfo& viewportCreateInfo);

    // populate a rasterization's create info.
    //
    // @param rasterizationCreateInfo stored populated rasterization create info.
    void populateRasterizationCreateInfo(VkPipelineRasterizationStateCreateInfo& rasterizationCreateInfo);

    // populate a multisampling's create info.
    //
    // @param multisamplingCreateInfo stored populated multisampling create info.
    void populateMultisamplingCreateInfo(VkPipelineMultisampleStateCreateInfo& multisamplingCreateInfo);

    // populate color blend components(color attachment and color blend's create info).
    //
    // @param colorblendAttachment stored populated color blend attachment.
    // @param colorBlendCreateInfo stored populated color blend create info.
    void populateColorBlendComponents(VkPipelineColorBlendAttachmentState& colorBlendAttachment, VkPipelineColorBlendStateCreateInfo& colorBlendCreateInfo);

    // populate a dynamic states's create info.
    //
    // @param dynamicStates dynamic states to use in populating.
    // @param dynamicStatesCreateInfo stored populated dynamic states create info.
    void populateDynamicStatesCreateInfo(std::vector<VkDynamicState>& dynamicStates, VkPipelineDynamicStateCreateInfo& dynamicStatesCreateInfo);

    // create a pipeline layout.
    void createMemberPipelineLayout();
    
    // create member Vulkan graphics pipeline.
    void createMemberGraphicsPipeline();

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
