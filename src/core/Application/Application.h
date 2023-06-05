#ifndef APPLICATION_H
#define APPLICATION_H

#include <core/Renderer/Renderer.h>
#include <core/VulkanInstance/VulkanInstance.h>


class Application
{
private:
    RendererDetails::Renderer m_renderer;
    VulkanInstance m_instance;
    
    DisplayManager::DisplayDetails m_displayDetails;
    
    
    // initializate the application and its dependencies.
    void initialize();

    // run the application and its dependencies.
    void run();
    
    // terminate the application and its dependencies.
    void terminate();
public:
    // launch the application.
    void launch();

    Application();
};


#endif  // APPLICATION_H
