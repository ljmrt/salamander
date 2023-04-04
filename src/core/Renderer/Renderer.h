#ifndef RENDERER_H
#define RENDERER_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/VulkanInstance.h>
#include <core/Config/Config.h>


class Renderer
{
private:
    GLFWwindow *m_window;  // current window in renderer context.
    ConfigDB m_windowConfig;  // window configuration database.

    VulkanInstance m_instance;  // current vulkan instance **class**.
    
    
    // initialize Vulkan.
    void vulkanInit();
    
    // render/main loop.
    void render();
    
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
