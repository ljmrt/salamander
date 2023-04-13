#ifndef APPLICATION_H
#define APPLICATION_H

#include <core/Renderer/Renderer.h>
#include <core/VulkanInstance/VulkanInstance.h>


class Application
{
private:
    Renderer m_renderer;
    VulkanInstance m_instance;
    
    DisplayManager::DisplayDetails m_displayDetails;
    
    
    // initializate the application.
    void initialize();

    // run the application/main loop.
    void run();
    
    // terminate the application.
    void terminate();
public:
    // launch the application.
    void launch();

    Application();
};


#endif  // APPLICATION_H
