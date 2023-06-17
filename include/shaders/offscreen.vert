#version 450

layout(binding = 0) uniform OffscreenUniformBufferObject {
    mat4 modelViewProjectionMatrix;
} offscreenUniformBufferObject;

layout(location = 0) in vec3 positionAttribute;

void main()
{
    gl_Position = (offscreenUniformBufferObject.modelViewProjectionMatrix * vec4(positionAttribute, 1.0));
}