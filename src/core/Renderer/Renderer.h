#ifndef RENDERER_H
#define RENDERER_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/VulkanInstance.h>
#include <core/Shader/Shader.h>


// TODO: move initialization, run, cleanup functions to higher-level application class?
class Renderer
{
private:
    Shader::ShaderStages m_shaderStages;  // graphics pipeline shader stages.

    const std::vector<VkDynamicState> m_dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    // TODO: reference to Vulkan instance's logical device?
    VkRenderPass m_renderPass;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;

    // create the renderer's render pass.
    //
    // @param vulkanLogicalDevice the Vulkan logical device to create the render pass with.
    // @param swapchainImageFormat the swapchain image format to use in render pass creation.
    void createRenderPass(VkDevice vulkanLogicalDevice, VkFormat swapchainImageFormat);
    
    // create the **Vulkan** graphics pipeline.
    //
    // @param vulkanLogicalDevice the Vulkan logical device to create the graphics pipeline with.
    void createGraphicsPipeline(VkDevice vulkanLogicalDevice);
public:
    // render/main loop.
    //
    // @param vulkanLogicalDevice the Vulkan logical device to render with.
    // @param displayDetails the display details to use in rendering.
    void render(VkDevice vulkanLogicalDevice, DisplayManager::DisplayDetails displayDetails);

    // terminates/destroys libraries, frees memory, etc.
    //
    // @param vulkanLogicalDevice the Vulkan logical device to use in renderer cleanup.
    void cleanupRenderer(VkDevice vulkanLogicalDevice);
    

    Renderer();
};


#endif  // RENDERER_H
