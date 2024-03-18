#ifndef APPLICATION_H
#define APPLICATION_H

#include <core/Renderer/Renderer.h>
#include <core/VulkanInstance/VulkanInstance.h>


class Application
{
private:
	// this order is important! member initializer lists initialize in order of declaration.
    VulkanInstance m_instance;
    RendererDetails::Renderer m_renderer;
    
    DisplayManager::DisplayDetails m_displayDetails;
    
public:
	Application();
    ~Application();

    // run the application and its dependencies.
    void run();
};


#endif  // APPLICATION_H
