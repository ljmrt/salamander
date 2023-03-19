#ifndef RENDERER_H
#define RENDERER_H


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
    // current window in renderer context.
    GLFWwindow *m_window;
    
    
    // initialize GLFW and create a window.
    void windowInit();
    
    // initialize vulkan.
    void vulkanInit();
    
    // render/main loop.
    void render();
    // process incoming input to the window.
    //
    // uses the currently assigned window.
    void render_process_input();
    
    // terminates/destroys libraries, frees memory, etc.
    void cleanup();
};


#endif  // RENDERER_H
