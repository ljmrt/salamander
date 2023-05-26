#include <core/Application/Application.h>
#include <core/Defaults/Defaults.h>
#include <core/DisplayManager/DisplayManager.h>
#include <core/Callbacks/Callbacks.h>


void Application::initialize()
{
    Defaults::initializeDefaults();

    DisplayManager::initializeGLFW();
    DisplayManager::createWindow(Defaults::windowDefaults.MAIN_WINDOW_WIDTH, Defaults::windowDefaults.MAIN_WINDOW_HEIGHT, Defaults::windowDefaults.MAIN_WINDOW_NAME, m_displayDetails.glfwWindow);

    glfwSetFramebufferSizeCallback(m_displayDetails.glfwWindow, Callbacks::glfwFramebufferResizeCallback);
    glfwSetMouseButtonCallback(m_displayDetails.glfwWindow, Callbacks::glfwMouseButtonCallback);
    glfwSetScrollCallback(m_displayDetails.glfwWindow, Callbacks::glfwMouseScrollCallback);

    m_instance = VulkanInstance(Defaults::windowDefaults.MAIN_WINDOW_NAME, m_displayDetails);
    m_renderer.setVulkanLogicalDevice(&m_instance.m_devices.logicalDevice);
}

void Application::run()
{
    m_renderer.render(m_displayDetails, m_instance.m_familyIndices.graphicsFamily.value(), m_instance.m_devices.physicalDevice);
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
