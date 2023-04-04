#include <core/Renderer/Renderer.h>
#include <core/Defaults/Defaults.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>


int main()
{
    Renderer application;

    try {
        Defaults::initializeDefaults();
        application.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
