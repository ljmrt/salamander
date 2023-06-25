cmake_minimum_required(VERSION 3.9.1)

project(salamander)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(PROJECT_OUTPUT_DIR ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_OUTPUT_DIR})
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(PROJECT_LIBRARIES_DIR ${CMAKE_SOURCE_DIR}/libs)

set(CMAKE_SKIP_INSTALL_RULES True)

if (!UNIX)
   message("WARNING: Salamander is only supported on Linux!")
endif()
if (${CMAKE_BUILD_TYPE} MATCHES Debug)
   set(CMAKE_CXX_FLAGS "-g -Wall -Werror -Wextra -O0 -Wno-unused-parameter -fsanitize=address,undefined")
endif()

###############################

find_package(Vulkan REQUIRED)

set(GLFW_DIR "${PROJECT_LIBRARIES_DIR}/glfw")
add_subdirectory(${GLFW_DIR} EXCLUDE_FROM_ALL)

set(GLM_DIR "${PROJECT_LIBRARIES_DIR}/glm")
add_subdirectory(${GLM_DIR} EXCLUDE_FROM_ALL)

set(STB_DIR "${PROJECT_LIBRARIES_DIR}/stb")
add_subdirectory(${STB_DIR} EXCLUDE_FROM_ALL)

set(TINYGLTF_DIR "${PROJECT_LIBRARIES_DIR}/tinygltf")
add_subdirectory(${TINYGLTF_DIR} EXCLUDE_FROM_ALL)
set_target_properties(tinygltf PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:tinygltf,INTERFACE_INCLUDE_DIRECTORIES>)  # ignores build errors in tinygltf

include_directories(${PROJECT_SOURCE_DIR})

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/core/Application/Application.cpp
    ${PROJECT_SOURCE_DIR}/core/Logging/ErrorLogger.cpp
    ${PROJECT_SOURCE_DIR}/core/Logging/DebugMessenger.cpp
    ${PROJECT_SOURCE_DIR}/core/Renderer/Renderer.cpp
    ${PROJECT_SOURCE_DIR}/core/Renderer/Pipeline.cpp
    ${PROJECT_SOURCE_DIR}/core/Renderer/Offscreen.cpp
    ${PROJECT_SOURCE_DIR}/core/Shader/Shader.cpp
    ${PROJECT_SOURCE_DIR}/core/Shader/ResourceDescriptor.cpp
    ${PROJECT_SOURCE_DIR}/core/Shader/Uniform.cpp
    ${PROJECT_SOURCE_DIR}/core/Shader/Image.cpp
    ${PROJECT_SOURCE_DIR}/core/Shader/Depth.cpp
    ${PROJECT_SOURCE_DIR}/core/Buffer/Buffer.cpp
    ${PROJECT_SOURCE_DIR}/core/Model/ModelHandler.cpp
    ${PROJECT_SOURCE_DIR}/core/DisplayManager/DisplayManager.cpp
    ${PROJECT_SOURCE_DIR}/core/DisplayManager/Camera.cpp
    ${PROJECT_SOURCE_DIR}/core/DisplayManager/SwapchainHandler.cpp
    ${PROJECT_SOURCE_DIR}/core/Command/CommandManager.cpp
    ${PROJECT_SOURCE_DIR}/core/VulkanExtensions/VulkanExtensions.cpp
    ${PROJECT_SOURCE_DIR}/core/VulkanInstance/SupportUtils.cpp
    ${PROJECT_SOURCE_DIR}/core/VulkanInstance/DeviceHandler.cpp
    ${PROJECT_SOURCE_DIR}/core/VulkanInstance/VulkanInstance.cpp
    ${PROJECT_SOURCE_DIR}/core/Queue/Queue.cpp
    ${PROJECT_SOURCE_DIR}/core/Callbacks/Callbacks.cpp
    ${PROJECT_SOURCE_DIR}/core/Config/Config.cpp
    ${PROJECT_SOURCE_DIR}/core/Defaults/Defaults.cpp
    ${PROJECT_SOURCE_DIR}/utils/FileUtils.cpp
    ${PROJECT_SOURCE_DIR}/utils/MathUtils.cpp
    ${PROJECT_SOURCE_DIR}/main.cpp
)

target_link_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        ${Vulkan_LIBRARY}
        glfw
        glm
        stb
        tinygltf
)
