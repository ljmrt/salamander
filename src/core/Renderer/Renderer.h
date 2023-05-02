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
    VkPipelineLayout m_pipelineLayout;  // this graphics pipeline's pipeline layout.
    VkPipeline m_graphicsPipeline;  // graphics pipeline

    VkCommandPool m_graphicsCommandPool;  // a command pool used for graphics command buffers.
    std::vector<VkCommandBuffer> m_graphicsCommandBuffers;  // child command buffers under the graphics command pool.

    // TODO: add vertexHandler struct of data buffers.
    VkBuffer m_vertexBuffer;  // vertex buffer.
    VkDeviceMemory m_vertexBufferMemory;  // the vertex buffer's memory.
    VkBuffer m_indexBuffer;  // index buffer.
    VkDeviceMemory m_indexBufferMemory;  // the index buffer's memory.

    std::vector<VkVertexInputAttributeDescription> m_attributeDescriptions;  // preserved attribute descriptions.
    VkVertexInputBindingDescription m_bindingDescription;  // preserved binding description.

    std::vector<VkSemaphore> m_imageAvailibleSemaphores;  // semaphore used to make the GPU wait to continue until the next availible image index in the swapchain has been fetched.
    std::vector<VkSemaphore> m_renderFinishedSemaphores;  // semaphore used to make the GPU wait to continue until the current frame has finished rendering.
    std::vector<VkFence> m_inFlightFences;  // fence used to synchronize the GPU and CPU before begining to draw another frame.]

    size_t m_currentFrame = 0;  // the current "frame" in context of the "in flight" frames.
    

    // fill out a color attachment description and reference.
    //
    // @param swapchainImageFormat the image format of the swapchain, used in filling out as the color attachment is ultimately sent to the swapchain.
    // @param colorAttachmentDescription stored filled color attachment description.
    // @param colorAttachmentReference stored filled color attachment reference.
    void fillColorAttachment(VkFormat swapchainImageFormat, VkAttachmentDescription& colorAttachmentDescription, VkAttachmentReference& colorAttachmentReference);
    
    // fill out a subpass's description.
    //
    // @param colorAttachmentReference color attachment reference to use in subpass description.
    // @param subpassDescription stored filled subpass description.
    void fillSubpassDescription(VkAttachmentReference *colorAttachmentReference, VkSubpassDescription& subpassDescription);
    
    // create member render pass.
    //
    // @param swapchainImageFormat the swapchain image format to use in render pass creation.
    void createMemberRenderPass(VkFormat swapchainImageFormat);

    // create member descriptor set layout.
    void createMemberDescriptorSetLayout();

    // fill out a vertex input's create info.
    //
    // @param vertexInputCreateInfo stored filled vertex input create info.
    void fillVertexInputCreateInfo(VkPipelineVertexInputStateCreateInfo& vertexInputCreateInfo);

    // fill out a input assembly's create info.
    //
    // @param inputAssemblyCreateInfo stored filled input assembly create info.
    void fillInputAssemblyCreateInfo(VkPipelineInputAssemblyStateCreateInfo& inputAssemblyCreateInfo);

    // fill out a viewport's create info.
    //
    // @param viewportCreateInfo stored filled viewport create info.
    void fillViewportCreateInfo(VkPipelineViewportStateCreateInfo& viewportCreateInfo);

    // fill out a rasterization's create info.
    //
    // @param rasterizationCreateInfo stored filled rasterization create info.
    void fillRasterizationCreateInfo(VkPipelineRasterizationStateCreateInfo& rasterizationCreateInfo);

    // fill out a multisampling's create info.
    //
    // @param multisamplingCreateInfo stored filled multisampling create info.
    void fillMultisamplingCreateInfo(VkPipelineMultisampleStateCreateInfo& multisamplingCreateInfo);

    // fill out a color blend attachment and color blend's create info.
    //
    // @param colorBlendAttachment stored filled color blend attachment.
    // @param colorBlendCreateInfo stored filled color blend create info.
    void fillColorBlend(VkPipelineColorBlendAttachmentState& colorBlendAttachment, VkPipelineColorBlendStateCreateInfo& colorBlendCreateInfo);

    // fill out a dynamic states's create info.
    //
    // @param dynamicStates dynamic states to use in filling.
    // @param dynamicStatesCreateInfo stored filled dynamic states create info.
    void fillDynamicStatesCreateInfo(std::vector<VkDynamicState>& dynamicStates, VkPipelineDynamicStateCreateInfo& dynamicStatesCreateInfo);

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
