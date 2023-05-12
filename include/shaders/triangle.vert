#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
} uniformBufferObject;

layout(location = 0) in vec2 positionAttribute;
layout(location = 1) in vec3 colorAttribute;
layout(location = 2) in vec2 textureCoordinatesAttribute;

layout(location = 0) out vec3 fragmentColor;
layout(location = 1) out vec2 fragmentTextureCoordinates;

void main()
{
    gl_Position = uniformBufferObject.projectionMatrix * uniformBufferObject.viewMatrix * uniformBufferObject.modelMatrix * vec4(positionAttribute, 0.0, 1.0);
    fragmentColor = colorAttribute;
    fragmentTextureCoordinates = textureCoordinatesAttribute;
}