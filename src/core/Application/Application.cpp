#include <core/Application/Application.h>
#include <core/Defaults/Defaults.h>
#include <core/DisplayManager/DisplayManager.h>
#include <core/Callbacks/Callbacks.h>


Application::Application() : m_instance(Defaults::windowDefaults.MAIN_WINDOW_NAME, m_displayDetails), m_renderer(m_displayDetails, m_instance.m_familyIndices.graphicsFamily.value(), m_instance.m_devices.physicalDevice)
{
    Defaults::initializeDefaults();

    DisplayManager::initializeGLFW();
    DisplayManager::createWindow(Defaults::windowDefaults.MAIN_WINDOW_WIDTH, Defaults::windowDefaults.MAIN_WINDOW_HEIGHT, Defaults::windowDefaults.MAIN_WINDOW_NAME, m_displayDetails.glfwWindow);

    glfwSetFramebufferSizeCallback(m_displayDetails.glfwWindow, Callbacks::glfwFramebufferResizeCallback);
    glfwSetMouseButtonCallback(m_displayDetails.glfwWindow, Callbacks::glfwMouseButtonCallback);
    glfwSetScrollCallback(m_displayDetails.glfwWindow, Callbacks::glfwMouseScrollCallback);

    m_renderer.setVulkanLogicalDevice(&m_instance.m_devices.logicalDevice);
}

void Application::run()
{
    m_renderer.run(m_displayDetails, m_instance.m_devices.physicalDevice);
}

Application::~Application()
{
	DisplayManager::cleanupGLFW(m_displayDetails.glfwWindow);
    // cleanup variables specified in Defaults::ApplicationCleanup.
    for (VkVertexInputBindingDescription *bindingDescriptionPointer : Defaults::applicationCleanup.vertexInputBindingDescriptionsMemory) {
        delete bindingDescriptionPointer;
    }

	// rest of the variables destructed automatically.
}
