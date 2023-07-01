#version 450

layout(binding = 0) uniform PointShadowUniformBufferObject {
    mat4 shadowTransforms[6];
    mat4 modelMatrix;
} uniformBufferObject;

layout(location = 0) in vec3 positionAttribute;

void main()
{
    gl_Position = (uniformBufferObject.modelMatrix * vec4(positionAttribute, 1.0));
}