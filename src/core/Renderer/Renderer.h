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

    VkPipelineLayout m_pipelineLayout;

    // create the **Vulkan** graphics pipeline.
    void createGraphicsPipeline(VkDevice vulkanLogicalDevice);
public:
    // render/main loop.
    void render(VkDevice vulkanLogicalDevice, DisplayManager::DisplayDetails displayDetails);

    // terminates/destroys libraries, frees memory, etc.
    void cleanupRenderer(VkDevice vulkanLogicalDevice);
    

    Renderer();
};


#endif  // RENDERER_H
