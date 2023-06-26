#ifndef DEFAULTS_H
#define DEFAULTS_H

#include <core/DisplayManager/Camera.h>
#include <core/Config/Config.h>


namespace Defaults  // NOTE: a "default" in this sense is treated and used as a global variable, generally read from pre-determined configuration files.
{
    namespace
    {
        ConfigDB m_windowDatabase;
        ConfigDB m_rendererDatabase;
        ConfigDB m_loggingDatabase;
    }

    // read from the window configuration file.
    struct WindowConfig {
        size_t MAIN_WINDOW_WIDTH;  // width of the main displayed window.
        size_t MAIN_WINDOW_HEIGHT;  // height of the main displayed window.
        std::string MAIN_WINDOW_NAME;  // name of the main displayed window.
    };
    extern WindowConfig windowDefaults;  // default/read window configuration.

    // read from the renderer configuration file.
    struct RendererConfig {
        uint32_t MAX_FRAMES_IN_FLIGHT;  // the max amount of frames that can be "in flight" or concurrently worked on by the CPU.
        float MAIN_CAMERA_ZOOM_AMOUNT;  // the amount that the main camera zooms in during one scroll callback.
    };
    extern RendererConfig rendererDefaults;  // default/read renderer configuration.

    // read/dynamically set from callbacks.
    struct CallbacksVariables {
        bool FRAMEBUFFER_RESIZED;  // dynamically set from callback: if the framebuffer has been resized(some platforms or drivers may not trigger swapchain recreation properly).
        Camera::ArcballCamera *MAIN_CAMERA;
    };
    extern CallbacksVariables callbacksVariables;  // dynamically set variables to input and output from callbacks.

    // read from various sources.
    struct ApplicationConfig {
        std::string SALAMANDER_ROOT_DIRECTORY;  // the "root" directory of the project(ex: /home/lucas/salamander/), read from the %SALAMANDER_ROOT% environment variable.
    };
    extern ApplicationConfig applicationDefaults;

    // TODO: can this be improved/eliminated with smart pointers?
    struct ApplicationCleanup {  // variables that are "cleaned up" or deleted before application exit(think of as sending something to /dev/null).
        std::vector<VkVertexInputBindingDescription *> vertexInputBindingDescriptionsMemory;
    };
    extern ApplicationCleanup applicationCleanup;
    

    // initialize all of the defaults.
    void initializeDefaults();
}


#endif  // DEFAULTS_H
