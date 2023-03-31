#ifndef RENDERER_H
#define RENDERER_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/VulkanInstance/VulkanInstance.h>
#include <core/Config/Config.h>


class Renderer
{
private:
    uint32_t WINDOW_WIDTH;  // height of the displayed window;
    uint32_t WINDOW_HEIGHT;  // width of the displayed window;
    std::string WINDOW_NAME;  // name to be displayed and used in the application.
    GLFWwindow *m_window;  // current window in renderer context.
    
    ConfigDB m_windowConfig;  // window configuration database.
    

    VulkanInstance m_instance;  // current vulkan instance **class**.
    
    
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
public:
    // run the renderer.
    //
    // handles vulkan initialization, rendering, and cleanup.
    void run();

    Renderer();
};


#endif  // RENDERER_H
