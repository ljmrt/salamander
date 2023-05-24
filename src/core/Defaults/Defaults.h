#ifndef DEFAULTS_H
#define DEFAULTS_H

#include <core/DisplayManager/Camera.h>
#include <core/Config/Config.h>


// TODO: move outside "/core"?
namespace Defaults
{
    namespace
    {
        ConfigDB m_windowDatabase;
        ConfigDB m_rendererDatabase;
        ConfigDB m_loggingDatabase;
    }
    
    // window configuration.
    struct windowConfig {
        size_t MAIN_WINDOW_WIDTH;  // width of the main displayed window.
        size_t MAIN_WINDOW_HEIGHT;  // height of the main displayed window.
        std::string MAIN_WINDOW_NAME;  // name of the main displayed window.
    };
    extern windowConfig windowDefaults;  // default/read window configuration.

    // renderer configuration.
    struct rendererConfig {
        size_t MAX_FRAMES_IN_FLIGHT;  // the max amount of frames that can be "in flight" or concurrently worked on by the CPU.
    };
    extern rendererConfig rendererDefaults;  // default/read renderer configuration.

    // logging configuration.

    // callbacks variables.
    struct callbacksConfig {
        bool FRAMEBUFFER_RESIZED;  // dynamically set from callback: if the framebuffer has been resized(some platforms or drivers may not trigger swapchain recreation properly).
        Camera::ArcballCamera *MAIN_CAMERA;
    };
    extern callbacksConfig callbacksVariables;  // dynamically set variables to input and output from callbacks.

    // misc. configuration.
    struct miscConfig {
        std::string SALAMANDER_ROOT_DIRECTORY;  // the "root" directory of the project(ex: /home/lucas/salamander/), read from the %SALAMANDER_ROOT% environment variable.
    };
    extern miscConfig miscDefaults;  // default/read misc. configuration.
    

    // initialize all of the defaults.
    void initializeDefaults();
}


#endif  // DEFAULTS_H
