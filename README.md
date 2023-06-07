<h1 align="center">
    <br>
    <img src="https://github.com/ljmrt/salamander/blob/ee50dac4b6d9a2d4f5200d80055a1c4f4d843450/assets/branding/logo3000x1688.png" alt="Salamander" width="750" height="422">
    <br>
    Salamander
    <br>
</h1>
<p align="center">
    <img src="https://img.shields.io/static/v1?label=release-version&message=1.1.1&color=green">
    <img src="https://img.shields.io/static/v1?label=build-version&message=1.1.1&color=green">
    <img src="https://img.shields.io/static/v1?label=language&message=C%2B%2B17&color=green">
    <img src="https://img.shields.io/static/v1?label=platform&message=Linux&color=green">
    <img src="https://img.shields.io/static/v1?label=development&message=Active&color=green">
</p>
<p align="center">Modern Vulkan based real-time renderer written in C++17.</p>

## Notice

This is the "development" branch! Most builds here will run, but may not be fully finished. Stable releases can be found on the "release" branch, but may not be as bleeding-edge as this.

## Features

WIP

## Disclaimer

Some features or parts of the engine may not be fully implemented and bugs will occur due to the work-in-progress nature of this project.

## Screenshots

WIP

## Project building and setup

Note: Currently only supported on Linux.

### Dependencies

[GLFW](https://github.com/glfw/glfw), [GLM](https://github.com/g-truc/glm), [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h), and [tinygltf](https://github.com/syoyo/tinygltf) are all included in the project's source code. Vulkan tools, layers, and loader will need to be installed by your package manager. CMake and similar build tools may need to be installed if not already.

### Compiling builds

To compile debug or release builds, you must be in the "build" directory and run the corresponding build script(compileDebug.sh, compileRelease.sh).

### Compiled binaries

Compiled debug or release binaries can be found in the "bin" directory.

### Setup

The environment variable %SALAMANDER_ROOT% has to point to Salamander's root directory("." relative to this file) for proper functionality. This can be set manually, or you can run "setupenv.sh" to automatically set up the environment variable(appends export to your ".bashrc"). In addition, see the "config" directory to view project configuration.

### Controls

* Right mouse button: Arcball camera control.
* Mouse scroll: camera zoom.
* "R" keypress: reset camera orientation.

###

## Project structure

```diff
.
├── assets                    # Misc. and outside-project files.
│   ├── branding              # Project branding(logos, etc.).
│   ├── models                # (glTF) Default models and model storage directory.
│   └── textures              # Misc. textures.
├── bin                       # (Not included by the repository, created by script) Output binary files.
├── build                     # Compile debug or release mode scripts(.sh), CMake output directory.
├── include                   # Seperated project-related files.
│   ├── config                # Project configuration.
│   └── shaders               # Shaders used internally.
├── libs                      # Third-party libraries/dependencies.
│   ├── glfw                  # Used for windowing and input support.
│   ├── glm                   # Used for mathematics and useful graphics functions.
│   ├── stb                   # Used for texture image loading.
│   └── tinygltf              # Used for glTF model loading.
└── src                       # C++ implementation and header files.
    ├── core                  # Fundamental project code.
    │   ├── Application       # Highest-level application manager.
    │   ├── Buffer            # Vulkan buffer-related functions and code.
    │   ├── Callbacks         # Windowing, input, rendering, etc. callbacks.
    │   ├── Command           # Vulkan command handling.
    │   ├── Config            # (.scfg) Configuration system.
    │   ├── Defaults          # Global/"default" variables.
    │   ├── DisplayManager    # Display-related functions and code.
    │   ├── Logging           # Core error logging and handling systems.
    │   ├── Model             # Model loading and processing system.
    │   ├── Queue             # Queue and queue family related functions and code.
    │   ├── Renderer          # The core renderer.
    │   ├── Shader            # Shader-related functions and code.
    │   ├── VulkanExtensions  # Functions/features not included-by-default in Vulkan.
    │   └── VulkanInstance    # Vulkan setup and related functions and code.
    └── utils                 # Utility functions.
```

## References

[Vulkan Tutorial](https://vulkan-tutorial.com/): Vulkan guidance.

[SaferGo/CroissantVulkanRenderer](https://github.com/SaferGo/CroissantVulkanRenderer): Project inspiration.

## Included assets

* [Avocado](https://github.com/KhronosGroup/glTF-Sample-Models/tree/4ca06672ce15d6a27bfb5cf14459bc52fd9044d1/2.0/Avocado)

## License

Distributed under the Gnu General Public License. See ['LICENSE'](https://github.com/ljmrt/salamander/blob/master/LICENSE) for more info.
