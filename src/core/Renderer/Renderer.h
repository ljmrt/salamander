#ifndef RENDERER_H
#define RENDERER_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/DeviceHandler.h>
#include <core/Shader/Shader.h>
#include <core/DisplayManager/Camera.h>
#include <core/Model/ModelHandler.h>

#include <vector>


namespace DisplayManager  // forward declaration.
{
    struct VulkanDisplayDetails;
    struct DisplayDetails;
}

namespace RendererDetails
{
    struct PipelineComponents {        
        VkDescriptorSetLayout descriptorSetLayout;  // this pipeline layout's descriptor set layout.
        VkDescriptorPool descriptorPool;  // the descriptor pool to use in scene descriptor set creation.
        std::vector<VkDescriptorSet> descriptorSets;  // uniform buffer descriptor sets.

        std::vector<VkBuffer> uniformBuffers;  // the uniform buffers specified for the pipeline.
        std::vector<VkDeviceMemory> uniformBuffersMemory;  // the uniform buffers' memory.
        std::vector<void *> mappedUniformBuffersMemory;  // the mapped memory of the uniform buffers.

        bool imageDetailsUsed = false;  // if the above image details are used at all.
    
        VkPipelineLayout pipelineLayout;  // this pipeline's pipeline layout.
        VkPipeline pipeline;  // the pipeline.

        Shader::PipelineShaders pipelineShaders;  // pipeline shaders/shader stages.

        
        void cleanupPipelineComponents(VkDevice vulkanLogicalDevice);
    };

    class Renderer
    {
    private:
        VkDevice *m_vulkanLogicalDevice;  // pointer to this application's Vulkan instance.
    
        PipelineComponents m_cubemapPipelineComponents;  // the components used in the cubemap's graphics pipeline.    
        PipelineComponents m_scenePipelineComponents;  // the components used in the scene's graphics pipeline.

        VkRenderPass m_renderPass;  // we only need a singular render pass.

        std::vector<VkSemaphore> m_imageAvailibleSemaphores;  // semaphore used to make the GPU wait to continue until the next availible image index in the swapchain has been fetched.
        std::vector<VkSemaphore> m_renderFinishedSemaphores;  // semaphore used to make the GPU wait to continue until the current frame has finished rendering.
        std::vector<VkFence> m_inFlightFences;  // fence used to synchronize the GPU and CPU before begining to draw another frame.]

        uint32_t m_currentFrame = 0;  // the current "frame" in context of the "in flight" frames.

        Camera::ArcballCamera m_mainCamera;  // the scene's main camera.
        ModelHandler::Model m_mainModel;  // the main loaded model.

        ModelHandler::Model m_cubemapModel;  // the loaded model used for the cubemap.
        Image::TextureDetails m_cubemapTextureDetails;  // the cubemap's texture details.
    

        // populate a color attachment description, reference, color attachment resolve description, reference for the scene render pass.
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

        // create member render pass for all pipelines.
        //
        // @param swapchainImageFormat the swapchain image format to use in member scene render pass creation.
        // @param msaaSampleCount the amount of msaa samples.
        // @param vulkanPhysicalDevice Vulkan physical device to use in member scene render pass creation.
        void createMemberRenderPass(VkFormat swapchainImageFormat, VkSampleCountFlagBits msaaSampleCount, VkPhysicalDevice vulkanPhysicalDevice);

        // populate a viewport's create info.
        //
        // @param viewportCount see VkPipelineViewportStateCreateInfo documentation.
        // @param scissorCount see VkPipelineViewportStateCreateInfo documentation.
        // @param viewportCreateInfo populated viewport create info.
        void populateViewportCreateInfo(uint32_t viewportCount, uint32_t scissorCount, VkPipelineViewportStateCreateInfo& viewportCreateInfo);

        // populate a rasterization's create info.
        //
        // @param cullMode see VkPipelineRasterizationStateCreateInfo documentation.
        // @param frontFace see VkPipelineRasterizationStateCreateInfo documentation.
        // @param rasterizationCreateInfo populated rasterization create info.
        void populateRasterizationCreateInfo(VkCullModeFlags cullMode, VkFrontFace frontFace, VkPipelineRasterizationStateCreateInfo& rasterizationCreateInfo);

        // fetch the maximum usable sample count for a physical device.
        //
        // @param vulkanPhysicalDevice Vulkan physical device to use in maximum usable sample count fetch.
        // @param maximumUsableSampleCount the (fetched) maximum usable sample count.
        void fetchMaximumUsableSampleCount(VkPhysicalDevice vulkanPhysicalDevice, VkSampleCountFlagBits& maximumUsableSampleCount);

        // populate a multisampling's create info.
        //
        // @param rasterizationSamples see VkPipelineMultisampleStateCreateInfo documentation.
        // @param minSampleShading see VkPipelineMultisampleStateCreateInfo documentation.
        // @param multisamplingCreateInfo populated multisampling create info.
        void populateMultisamplingCreateInfo(VkSampleCountFlagBits rasterizationSamples, float minSampleShading, VkPipelineMultisampleStateCreateInfo& multisamplingCreateInfo);

        // populate a depth stencil's create info.
        //
        // @param depthTestEnable see VkPipelineDepthStencilStateCreateInfo documentation.
        // @param depthWriteEnable see VkPipelineDepthStencilStateCreateInfo documentation.
        // @param depthCompareOp see VkPipelineDepthStencilStateCreateInfo documentation.
        // @param depthStencilCreateInfo populated depth stencil create info.
        void populateDepthStencilCreateInfo(VkBool32 depthTestEnable, VkBool32 depthWriteEnable, VkCompareOp depthCompareOp, VkPipelineDepthStencilStateCreateInfo& depthStencilCreateInfo);

        // populate color blend components(color attachment and color blend's create info).
        //
        // @param colorWriteMask see VkPipelineColorBlendAttachmentState documentation.
        // @param blendEnable see VkPipelineColorBlendAttachmentState documentation.
        // @param colorblendAttachment populated color blend attachment.
        // @param colorBlendCreateInfo populated color blend create info.
        void populateColorBlendComponents(VkColorComponentFlags colorWriteMask, VkBool32 blendEnable, VkPipelineColorBlendAttachmentState& colorBlendAttachment, VkPipelineColorBlendStateCreateInfo& colorBlendCreateInfo);

        // populate a dynamic states's create info.
        //
        // @param dynamicStates dynamic states to use in populating.
        // @param dynamicStatesCreateInfo populated dynamic states create info.
        void populateDynamicStatesCreateInfo(std::vector<VkDynamicState>& dynamicStates, VkPipelineDynamicStateCreateInfo& dynamicStatesCreateInfo);

        // create a pipeline layout.
        //
        // @param descriptorSetLayout the descriptor set layout to use within the pipeline layout.
        // @param memberPipelineLayout the created member pipeline layout.
        void createMemberPipelineLayout(VkDescriptorSetLayout& descriptorSetLayout, VkPipelineLayout& memberPipelineLayout);

        // create member Vulkan cubemap pipeline.
        //
        // @param msaaSampleCount the amount of msaa samples.
        void createMemberCubemapPipeline(VkSampleCountFlagBits msaaSampleCount);
    
        // create member Vulkan scene pipeline.
        //
        // @param msaaSampleCount the amount of msaa samples.
        void createMemberScenePipeline(VkSampleCountFlagBits msaaSampleCount);

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
}


#endif  // RENDERER_H
