#version 450

layout(binding = 0) uniform CubemapUniformBufferObject {
    mat4 projectionMatrix;
    mat4 viewMatrix;
} cubemapUniformBufferObject;

layout(location = 0) in vec3 positionAttribute;
layout(location = 1) in vec3 normalAttribute;
layout(location = 2) in vec2 UVCoordinatesAttribute;

layout(location = 0) out vec3 fragmentCubemapUVCoordinates;

void main()
{
    gl_Position = (cubemapUniformBufferObject.projectionMatrix * cubemapUniformBufferObject.viewMatrix * vec4(positionAttribute, 1.0)).xyww;
    fragmentCubemapUVCoordinates = positionAttribute;
}