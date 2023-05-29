#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 projectionMatrix;
    mat4 viewMatrix;
} uniformBufferObject;

layout(location = 0) in vec3 positionAttribute;

layout(location = 0) out vec3 fragmentCubemapUVCoordinates;

void main()
{
    gl_Position = (uniformBufferObject.projectionMatrix * mat4(mat3(uniformBufferObject.viewMatrix)) * vec4(positionAttribute, 1.0));
    fragmentCubemapUVCoordinates = positionAttribute;
}