#version 450

layout(binding = 0) uniform CubemapUniformBufferObject {
    mat4 projectionMatrix;
    mat4 viewMatrix;
} cubemapUniformBufferObject;

layout(location = 0) in vec3 positionAttribute;

layout(location = 0) out vec3 fragmentCubemapUVCoordinates;

void main()
{
    gl_Position = (cubemapUniformBufferObject.projectionMatrix * mat4(mat3(cubemapUniformBufferObject.viewMatrix)) * vec4(positionAttribute, 1.0));
    fragmentCubemapUVCoordinates = positionAttribute;
}