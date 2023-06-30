#version 450

layout(binding = 0) uniform DirectionalShadowUniformBufferObject {
    mat4 lightSpaceMatrix;
    mat4 modelMatrix;
} uniformBufferObject;

layout(location = 0) in vec3 positionAttribute;

void main()
{
    gl_Position = (uniformBufferObject.lightSpaceMatrix * uniformBufferObject.modelMatrix * vec4(positionAttribute, 1.0));
}