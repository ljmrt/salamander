#version 450

layout(binding = 0) uniform OffscreenUniformBufferObject {
    mat4 lightSpaceMatrix;
} offscreenUniformBufferObject;

layout(location = 0) in vec3 positionAttribute;

void main()
{
    gl_Position = (offscreenUniformBufferObject.lightSpaceMatrix * vec4(positionAttribute, 1.0));
}