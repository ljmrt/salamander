#include <core/Defaults/Defaults.h>
#include <core/Config/Config.h>


Defaults::windowConfig Defaults::windowDefaults;

void Defaults::initializeDefaults()
{
    // initialize window defaults.
    m_windowDatabase = ConfigDB("/home/lucas/programming/graphics/salamander-engine/include/config/window.scfg");
    
    windowDefaults.MAIN_WINDOW_WIDTH = static_cast<uint32_t>(std::stoul(m_windowDatabase.lookupKey("MAIN_WINDOW_WIDTH")));
    windowDefaults.MAIN_WINDOW_HEIGHT = static_cast<uint32_t>(std::stoul(m_windowDatabase.lookupKey("MAIN_WINDOW_HEIGHT")));
    windowDefaults.MAIN_WINDOW_NAME = m_windowDatabase.lookupKey("MAIN_WINDOW_NAME");
    
    // initialize logging defaults.
    m_loggingDatabase = ConfigDB("/home/lucas/programming/salamander-engine/include/config/logging.scfg");
}
