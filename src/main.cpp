#include <core/Renderer/Renderer.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>


int main()
{
    Renderer application;

    try {
        application.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
