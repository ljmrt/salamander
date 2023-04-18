#include <core/Defaults/Defaults.h>
#include <core/Config/Config.h>

#include <string>
#include <iostream>
#include <cstdlib>


// extern variables from header file.
Defaults::windowConfig Defaults::windowDefaults;
Defaults::rendererConfig Defaults::rendererDefaults;
Defaults::miscConfig Defaults::miscDefaults;
Defaults::callbacksConfig Defaults::callbacksVariables;


void Defaults::initializeDefaults()
{
    // initialize misc. defaults.
    // no actual configuration needed yet.
    miscDefaults.SALAMANDER_ROOT_DIRECTORY = (std::string)std::getenv("SALAMANDER_ROOT");
    if (std::getenv("SALAMANDER_ROOT") == nullptr) {  // environment variable not set.
        std::cerr << "%SALAMANDER_ROOT% environment variable not set! See \"Setup\" section under \"README.md\"." << std::endl;  // we want this to output on both debug and release builds.
    }
    
    // initialize window defaults.
    m_windowDatabase = ConfigDB(Defaults::miscDefaults.SALAMANDER_ROOT_DIRECTORY + "/include/config/window.scfg");
    
    windowDefaults.MAIN_WINDOW_WIDTH = static_cast<size_t>(std::stoul(m_windowDatabase.lookupKey("MAIN_WINDOW_WIDTH")));
    windowDefaults.MAIN_WINDOW_HEIGHT = static_cast<size_t>(std::stoul(m_windowDatabase.lookupKey("MAIN_WINDOW_HEIGHT")));
    windowDefaults.MAIN_WINDOW_NAME = m_windowDatabase.lookupKey("MAIN_WINDOW_NAME");

    // initialize renderer defaults.
    m_rendererDatabase = ConfigDB(Defaults::miscDefaults.SALAMANDER_ROOT_DIRECTORY + "/include/config/renderer.scfg");

    rendererDefaults.MAX_FRAMES_IN_FLIGHT = static_cast<size_t>(std::stoul(m_rendererDatabase.lookupKey("MAX_FRAMES_IN_FLIGHT")));
    
    // initialize logging defaults.
    m_loggingDatabase = ConfigDB(Defaults::miscDefaults.SALAMANDER_ROOT_DIRECTORY + "/include/config/logging.scfg");

    // initialize callbacks variables.
    callbacksVariables.FRAMEBUFFER_RESIZED = false;
}
