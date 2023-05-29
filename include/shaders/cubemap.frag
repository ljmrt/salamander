#version 450

layout(binding = 0) uniform samplerCube cubemapTextureSampler;

layout(location = 0) in vec3 fragmentCubemapUVCoordinates;

layout(location = 0) out vec4 outputColor;

void main()
{
    outputColor = texture(cubemapTextureSampler, fragmentCubemapUVCoordinates);
}