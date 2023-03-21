#ifndef RENDERER_H
#define RENDERER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


class renderer
{
public:
    // run the renderer.
    //
    // handles vulkan initialization, rendering, and cleanup.
    void run();
private:
    const uint32_t WINDOW_WIDTH = 800;
    const uint32_t WINDOW_HEIGHT = 600;
    const char *WINDOW_NAME = "Salamander";
    GLFWwindow *m_window;  // current window in renderer context.

    VkInstance m_instance;  // current vulkan instance.
    
    
    // initialize GLFW and create a window.
    void windowInit();
    
    // initialize Vulkan.
    void vulkanInit();
    // create Vulkan instance.
    void vulkanCreateInstance();
    
    // render/main loop.
    void render();
    // process incoming input to the window.
    //
    // uses the currently assigned window.
    void renderProcessInput();
    
    // terminates/destroys libraries, frees memory, etc.
    void cleanup();
};


#endif  // RENDERER_H
