#include <core/Defaults/Defaults.h>
#include <core/Config/Config.h>

#include <string>
#include <iostream>
#include <cstdlib>


// extern variables from header file.
Defaults::WindowConfig Defaults::windowDefaults;
Defaults::RendererConfig Defaults::rendererDefaults;
Defaults::ApplicationConfig Defaults::applicationDefaults;
Defaults::CallbacksVariables Defaults::callbacksVariables;
Defaults::ApplicationCleanup Defaults::applicationCleanup;


void Defaults::initializeDefaults()
{
    // initialize misc. defaults.
    const char *rawRootDirectoryVariable = std::getenv("SALAMANDER_ROOT");
    if (!rawRootDirectoryVariable) {  // environment variable not set.
        std::cerr << "%SALAMANDER_ROOT% environment variable not set! See \"Setup\" section under \"README.md\"." << std::endl;  // we want this to output on both debug and release builds.
    } else {
        applicationDefaults.SALAMANDER_ROOT_DIRECTORY = (std::string)(rawRootDirectoryVariable);
    }
    
    
    // initialize window defaults.
    m_windowDatabase = ConfigDB(Defaults::applicationDefaults.SALAMANDER_ROOT_DIRECTORY + "/include/config/window.scfg");
    
    windowDefaults.MAIN_WINDOW_WIDTH = static_cast<size_t>(std::stoul(m_windowDatabase.lookupKey("MAIN_WINDOW_WIDTH")));
    windowDefaults.MAIN_WINDOW_HEIGHT = static_cast<size_t>(std::stoul(m_windowDatabase.lookupKey("MAIN_WINDOW_HEIGHT")));
    windowDefaults.MAIN_WINDOW_NAME = m_windowDatabase.lookupKey("MAIN_WINDOW_NAME");
    

    // initialize renderer defaults.
    m_rendererDatabase = ConfigDB(Defaults::applicationDefaults.SALAMANDER_ROOT_DIRECTORY + "/include/config/renderer.scfg");

    rendererDefaults.MAX_FRAMES_IN_FLIGHT = std::stoul(m_rendererDatabase.lookupKey("MAX_FRAMES_IN_FLIGHT"));
    rendererDefaults.MAIN_CAMERA_ZOOM_AMOUNT = std::stof(m_rendererDatabase.lookupKey("MAIN_CAMERA_ZOOM_AMOUNT"));
    
    
    // initialize logging defaults.
    m_loggingDatabase = ConfigDB(Defaults::applicationDefaults.SALAMANDER_ROOT_DIRECTORY + "/include/config/logging.scfg");

    // TODO: fully initialize/implement logging defaults.
    

    // initialize callbacks variables.
    callbacksVariables.FRAMEBUFFER_RESIZED = false;
}
