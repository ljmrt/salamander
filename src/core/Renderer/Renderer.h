#ifndef RENDERER_H
#define RENDERER_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/VulkanInstance.h>


class Renderer
{
private:
    VulkanInstance m_instance;  // current vulkan instance **class**.
    
    
    // initialize Vulkan.
    void vulkanInit();
    
    // render/main loop.
    void render();

    // create the **Vulkan** graphics pipeline.
    void createGraphicsPipeline();

    // TODO: seperate file/directory?
    // create a shader module using the supplied bytecode.
    //
    // @param shaderBytecode the shader's bytecode in a vector of chars.
    // @param resultShaderModule stored created shader module.
    void createShaderModule(const std::vector<char> shaderBytecode, VkShaderModule& resultShaderModule);
    
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
