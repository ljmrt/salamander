#include <core/Application/Application.h>
#include <core/Defaults/Defaults.h>
#include <core/DisplayManager/DisplayManager.h>


void Application::initialize()
{
    Defaults::initializeDefaults();

    DisplayManager::initializeGLFW();
    DisplayManager::createWindow(Defaults::windowDefaults.MAIN_WINDOW_WIDTH, Defaults::windowDefaults.MAIN_WINDOW_HEIGHT, Defaults::windowDefaults.MAIN_WINDOW_NAME, m_displayDetails.glfwWindow);

    m_instance = VulkanInstance(Defaults::windowDefaults.MAIN_WINDOW_NAME, m_displayDetails);
    m_renderer.setVulkanLogicalDevice(&m_instance.m_logicalDevice);
}

void Application::run()
{
    // TODO: only pass necessary Vulkan details.
    m_renderer.render(m_displayDetails);
}

void Application::terminate()
{
    m_renderer.cleanupRenderer();
    m_instance.cleanupInstance(m_displayDetails);
    DisplayManager::cleanupGLFW(m_displayDetails.glfwWindow);
}

void Application::launch()
{
    this->initialize();
    this->run();
    this->terminate();
}

Application::Application()
{

}
