#ifndef RENDERER_H
#define RENDERER_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// TODO: defaults namespace(window name, height[?] width[?])
#include <core/VulkanInstance/VulkanInstance.h>


class renderer
{
public:
    // run the renderer.
    //
    // handles vulkan initialization, rendering, and cleanup.
    void run();
private:
    const uint32_t WINDOW_WIDTH = 800;  // height of the displayed window;
    const uint32_t WINDOW_HEIGHT = 600;  // width of the displayed window;
    const char *WINDOW_NAME = "Salamander";  // name to be displayed and used in the application.
    GLFWwindow *m_window;  // current window in renderer context.

    VulkanInstance m_vkInstance;  // current vulkan instance **class**.
    
    
    // initialize GLFW and create a window.
    void windowInit();
    
    // initialize Vulkan.
    void vulkanInit();
    
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
