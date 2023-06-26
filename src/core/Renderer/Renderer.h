#ifndef RENDERER_H
#define RENDERER_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Renderer/Pipeline.h>
#include <core/Renderer/Offscreen.h>
#include <core/VulkanInstance/DeviceHandler.h>
#include <core/Shader/Shader.h>
#include <core/DisplayManager/Camera.h>
#include <core/Model/ModelHandler.h>

#include <vector>


namespace DisplayManager  // forward declaration.
{
    struct DisplayDetails;
}

namespace RendererDetails
{
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
    // @param storeOp the store operation to use for the depth attachment.
    // @param vulkanPhysicalDevice Vulkan physical device to use in depth attachment population.
    // @param attachment see VkAttachmentReference documentation.
    // @param depthAttachmentDescription populated depth attachment description.
    // @param depthAttachmentReference populated depth attachment reference.
    void populateDepthAttachmentComponents(VkSampleCountFlagBits msaaSampleCount, VkAttachmentStoreOp storeOp, uint32_t attachment, VkPhysicalDevice vulkanPhysicalDevice, VkAttachmentDescription& depthAttachmentDescription, VkAttachmentReference& depthAttachmentReference);

    // create a render pass for the directional shadow offscreen operation.
    //
    // @param vulkanDevices Vulkan physical and logical device to use in the directional shadow render pass creation.
    // @param renderPass created directional shadow render pass.
    void createDirectionalShadowRenderPass(DeviceHandler::VulkanDevices vulkanDevices, VkRenderPass& renderPass);

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
    // @param vulkanLogicalDevice Vulkan logical device to use in pipeline layout creation.
    // @param descriptorSetLayout the descriptor set layout to use within the pipeline layout.
    // @param pipelineLayout the created pipeline layout.
    void createPipelineLayout(VkDevice vulkanLogicalDevice, VkDescriptorSetLayout& descriptorSetLayout, VkPipelineLayout& pipelineLayout);

    // create directional shadow pipeline.
    //
    // @param renderPass render pass to use in directional shadow pipeline creation.
    // @param vulkanLogicalDevice Vulkan logical device to use in directional shadow pipeline creation.
    // @param pipelineComponents reference to the pipeline components to use and create the pipeline in.
    void createDirectionalShadowPipeline(VkRenderPass renderPass, VkDevice vulkanLogicalDevice, Pipeline::PipelineComponents& pipelineComponents);
    
    class Renderer
    {
    private:
        VkDevice *m_vulkanLogicalDevice;  // pointer to this application's Vulkan instance.
    
        Pipeline::PipelineComponents m_cubemapPipelineComponents;  // the components used in the cubemap's graphics pipeline.    
        Pipeline::PipelineComponents m_scenePipelineComponents;  // the components used in the scene's graphics pipeline.
        Pipeline::PipelineComponents m_sceneNormalsPipelineComponents;  // the components used in the scene normals' graphics pipeline.
        Offscreen::OffscreenOperation m_directionalShadowOperation;  // the pipeline components and similar used in the directional shadow mapping offscreen operation.

        VkRenderPass m_renderPass;  // we only need a singular render pass.

        std::vector<VkSemaphore> m_imageAvailibleSemaphores;  // semaphore used to make the GPU wait to continue until the next availible image index in the swapchain has been fetched.
        std::vector<VkSemaphore> m_renderFinishedSemaphores;  // semaphore used to make the GPU wait to continue until the current frame has finished rendering.
        std::vector<VkFence> m_inFlightFences;  // fence used to synchronize the GPU and CPU before begining to draw another frame.]

        uint32_t m_currentFrame = 0;  // the current "frame" in context of the "in flight" frames.

        Camera::ArcballCamera m_mainCamera;  // the scene's main camera.
        ModelHandler::Model m_mainModel;  // the main loaded model.
        ModelHandler::Model m_dummySceneNormalsModel;  // a "dummy" scene normals model used to populate the scene normals shader buffer components.
        ModelHandler::Model m_cubemapModel;  // the loaded model used for the cubemap.
        ModelHandler::Model m_dummyDirectionalShadowModel;  // a "dummy" model used to populate the directional shadow shader buffer components.


        // create member render pass for all pipelines.
        //
        // @param swapchainImageFormat the swapchain image format to use in member scene render pass creation.
        // @param msaaSampleCount the amount of msaa samples.
        // @param vulkanPhysicalDevice Vulkan physical device to use in member scene render pass creation.
        void createMemberRenderPass(VkFormat swapchainImageFormat, VkSampleCountFlagBits msaaSampleCount, VkPhysicalDevice vulkanPhysicalDevice);

        // create member cubemap pipeline.
        //
        // @param msaaSampleCount the amount of msaa samples.
        void createMemberCubemapPipeline(VkSampleCountFlagBits msaaSampleCount);
    
        // create member scene pipeline.
        //
        // @param msaaSampleCount the amount of msaa samples.
        void createMemberScenePipeline(VkSampleCountFlagBits msaaSampleCount);

        // create member scene normals pipeline.
        //
        // @param msaaSampleCount the amount of msaa samples.
        void createMemberSceneNormalsPipeline(VkSampleCountFlagBits msaaSampleCount);
        
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
        void render(DisplayManager::DisplayDetails& displayDetails, uint32_t graphicsFamilyIndex, VkPhysicalDevice vulkanPhysicalDevice);

        // terminates/destroys renderer and its members.
        void cleanupRenderer();

        // Renderer constructor.
        Renderer();
    };
}


#endif  // RENDERER_H
