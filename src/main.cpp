#include <core/Renderer/Renderer.h>
#include <core/Config/Config.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>


int main()
{
    Renderer application;
    ConfigDB configuration;

    try {
        configuration.loadConfig("/home/lucas/programming/graphics/salamander-engine/include/config/window.scfg");
        application.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
