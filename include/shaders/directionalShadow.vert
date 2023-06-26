#version 450

layout(binding = 0) uniform OffscreenUniformBufferObject {
    mat4 lightSpaceMatrix;
    mat4 modelMatrix;
} offscreenUniformBufferObject;

layout(location = 0) in vec3 positionAttribute;

void main()
{
    gl_Position = (offscreenUniformBufferObject.lightSpaceMatrix * offscreenUniformBufferObject.modelMatrix * vec4(positionAttribute, 1.0));
}