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
    VkPipelineLayout m_pipelineLayout;  // this graphics pipeline's pipeline layout.
    VkPipeline m_graphicsPipeline;  // graphics pipeline

    std::vector<VkFramebuffer> m_swapchainFramebuffers;  // framebuffers for all swapchain images views.

    VkCommandPool m_graphicsCommandPool;  // a command pool used for graphics command buffers.
    VkCommandBuffer m_graphicsCommandBuffer;  // child command buffer under the graphics command pool.

    VkSemaphore m_imageAvailibleSemaphore;  // semaphore used to make the GPU wait to continue until the next availible image index in the swapchain has been fetched.
    VkSemaphore m_renderFinishedSemaphore;
    VkFence m_inFlightFence;  // fence used to synchronize the GPU and CPU before begining to draw another frame.
    

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
    
    // create the renderer's render pass.
    //
    // @param swapchainImageFormat the swapchain image format to use in render pass creation.
    // @param renderPass stored created render pass.
    void createRenderPass(VkFormat swapchainImageFormat, VkRenderPass& renderPass);

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
    //
    // @param pipelineLayout stored created pipeline layout.
    void createPipelineLayout(VkPipelineLayout& pipelineLayout);
    
    // create the **Vulkan** graphics pipeline.
    //
    // @param graphicsPipeline stored created graphics pipeline.
    void createGraphicsPipeline(VkRenderPass renderPass, VkPipeline& graphicsPipeline);

    void createSynchronizationObjects(VkSemaphore& imageAvailibleSemaphore, VkSemaphore& renderFinishedSemaphore, VkFence& inFlightFence);

    void drawFrame();
public:
    // set the pointer to the Vulkan instance's logical device.
    //
    // @param vulkanLogicalDevice pointer to set personal pointer to.
    void setVulkanLogicalDevice(VkDevice *vulkanLogicalDevice);
    
    // render/main loop.
    //
    // @param displayDetails the display details to use in rendering.
    // @param graphicsFamilyIndex index of the graphics queue family.
    void render(DisplayManager::DisplayDetails displayDetails, size_t graphicsFamilyIndex);

    // terminates/destroys libraries, frees memory, etc.
    void cleanupRenderer();

    // Renderer constructor.
    Renderer();
};


#endif  // RENDERER_H
