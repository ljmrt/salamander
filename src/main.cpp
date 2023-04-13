#include <core/Application/Application.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>


int main()
{
    Application salamanderEngine;

    try {
        salamanderEngine.launch();
    } catch (const std::exception& e) {
        // std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
