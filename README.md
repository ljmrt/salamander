<h1 align="center">
    <br>
    <img src="https://github.com/ljmrt/salamander/blob/ee50dac4b6d9a2d4f5200d80055a1c4f4d843450/assets/branding/logo3000x1688.png" alt="Salamander" width="750" height="422">
    <br>
    Salamander
    <br>
</h1>
<p align="center">
    <img src="https://img.shields.io/static/v1?label=release-version&message=0.0.0&color=green">
    <img src="https://img.shields.io/static/v1?label=build-version&message=0.4.0&color=green">
    <img src="https://img.shields.io/static/v1?label=language&message=C%2B%2B17&color=green">
    <img src="https://img.shields.io/static/v1?label=platform&message=Linux&color=green">
    <img src="https://img.shields.io/static/v1?label=development&message=Active&color=green">
</p>
<p align="center">Modern Vulkan based real-time renderer written in C++17.</p>

## Features

WIP

## Disclaimer

Some features or parts of the engine may not be fully implemented and bugs will occur due to the work-in-progress nature of this project.

## Screenshots

WIP

## Usage

WIP

## Project structure

```diff
.
├── assets
│   ├── branding
│   ├── models
│   └── textures
├── build
├── include                  # Project configuration or read-from files.
│   └── config               # Configuration files.
├── libs                     # Third-party libraries/dependencies.
│   ├── glfw                 # GLFW: Used for windowing and input support.
│   └── glm                  # GLM: Used for mathematics and useful graphics functions.
├── src                      # C++ implementation files.
│   ├── core                 # Fundamental engine code/files(rendering, logging, etc.).
│   │   ├── Callbacks        # Windowing, input, rendering, etc. callbacks.
│   │   ├── Logging          # Core error logging and handling system(only enabled in debug builds).
│   │   ├── Renderer         # The core renderer.
│   │   ├── VulkanExtensions # Functions not included-by-default in Vulkan(but are loaded in).
│   │   └── VulkanInstance   # Vulkan instancing code.
│   └── extensions           # Features built on top of the core(GUI, image loading, etc.).
```

## Third-party libraries

* WIP

## Building

Currently Linux only, not many plans to move this elsewhere.
### Linux
#### Dependencies
GLFW and GLM are currently included in the project's source code as git submodules. No prerequesites are necessary.
#### Build commands
```diff

WIP

```
Built binaries should be found in the bin directory.

## References

[Vulkan Tutorial](https://vulkan-tutorial.com/): Vulkan guidance.

[SaferGo/CroissantVulkanRenderer](https://github.com/SaferGo/CroissantVulkanRenderer): Project inspiration.

## Included assets

WIP

## License

Distributed under the MIT License. See ['LICENSE'](https://github.com/ljmrt/salamander/blob/master/LICENSE) for more info.
