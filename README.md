<h1 align="center">
    <br>
    <img src="https://github.com/ljmrt/salamander/blob/ee50dac4b6d9a2d4f5200d80055a1c4f4d843450/assets/branding/logo3000x1688.png" alt="Salamander" width="750" height="422">
    <br>
    Salamander
    <br>
</h1>
<p align="center">
    <img src="https://img.shields.io/static/v1?label=release-version&message=1.0.3&color=green">
    <img src="https://img.shields.io/static/v1?label=build-version&message=1.0.3&color=green">
    <img src="https://img.shields.io/static/v1?label=language&message=C%2B%2B17&color=green">
    <img src="https://img.shields.io/static/v1?label=platform&message=Linux&color=green">
    <img src="https://img.shields.io/static/v1?label=development&message=Active&color=green">
</p>
<p align="center">Modern Vulkan based real-time renderer written in C++17.</p>

## Notice

This is the "release" branch! Fully released features and builds will be here. However, this may not be as cutting-edge as the "development" branch, but anything notable and stable will be released here.

## Features

WIP

## Disclaimer

Some features or parts of the engine may not be fully implemented and bugs will occur due to the work-in-progress nature of this project.

## Screenshots

WIP

## Project setup and building

Note: Currently only supported on Linux.

### Setup

The environment variable %SALAMANDER_ROOT% has to point to Salamander's root directory("." relative to this file) for proper functionality. This can be set manually, or you can run "setupenv.sh" to automatically set up the environment variable(appends export to your ".bashrc"). In addition, see the "config" directory to view project configuration.

### Dependencies

GLFW and GLM are included in the project's source code. Vulkan tools, layers, and loader will need to be installed by your package manager. CMake and similar build tools may need to be installed if not already.

### Compiling builds

To compile debug or release builds, you must be in the "build" directory and run the corresponding build script(compileDebug.sh, compileRelease.sh).

### Compiled binaries

Compiled debug or release binaries can be found in the "bin" directory.

## Project structure

```diff
.
├── assets                    # Misc. outside project files.
│   ├── branding              # Project branding(logos, etc.).
│   ├── models                # (.glTF) Default models and model storage directory.
│   └── textures              # Model and misc. textures.
├── bin                       # (Not included by repository, created by script) Output binary files.
├── build                     # Build debug or release mode script(.sh), CMake output file directory.
├── include                   # Project configuration or read-from files.
│   ├── config                # Configuration files.
│   └── shaders               # .vert and .frag shaders.
├── libs                      # Third-party libraries/dependencies.
│   ├── glfw                  # Used for windowing and input support.
│   └── glm                   # Used for mathematics and useful graphics functions.
└── src                       # C++ implementation files.
    ├── core                  # Fundamental engine code(rendering, logging, configuration, etc.).
    │   ├── Application       # Highest-level application, manages renderer and Vulkan instance.
    │   ├── Callbacks         # Windowing, input, rendering, etc. callbacks.
    │   ├── Config            # .scfg configuration system.
    │   ├── Defaults          # Global/default variables(window width, window height, etc.).
    │   ├── DisplayManager    # Display-related functions and handlers.
    │   ├── Logging           # Core error logging and handling systems(currently only enabled in debug builds).
    │   ├── Queue             # Queue and queue family code.
    │   ├── Renderer          # The core renderer/the graphics pipeline container.
    │   ├── Shader            # Shader structs and handlers.
    │   ├── VulkanExtensions  # Functions/features not included-by-default in Vulkan(loaded in here).
    │   └── VulkanInstance    # Core Vulkan processes/Vulkan instancing code.
    ├── extensions            # Features built on top of the core(GUI, image loading, etc.).
    └── utils                 # Utility functions(file loading, etc.).
```

## References

[Vulkan Tutorial](https://vulkan-tutorial.com/): Vulkan guidance.

[SaferGo/CroissantVulkanRenderer](https://github.com/SaferGo/CroissantVulkanRenderer): Project inspiration.

## Included assets

WIP

## License

Distributed under the MIT License. See ['LICENSE'](https://github.com/ljmrt/salamander/blob/master/LICENSE) for more info.
