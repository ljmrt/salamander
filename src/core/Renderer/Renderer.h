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
    VulkanInstance m_instance;  // current vulkan instance **class**.

    Shader::ShaderStages m_shaderStages;  // graphics pipeline shader stages.

    const std::vector<VkDynamicState> m_dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineLayout m_pipelineLayout;
    
    
    // initialize Vulkan.
    void vulkanInit();
    
    // render/main loop.
    void render();

    // create the **Vulkan** graphics pipeline.
    void createGraphicsPipeline();

    // terminates/destroys libraries, frees memory, etc.
    void cleanup();
public:
    // run the renderer.
    //
    // handles vulkan initialization, rendering, and cleanup.
    void run();

    Renderer();
};


#endif  // RENDERER_H
