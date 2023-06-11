#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 projectionMatrix;
    mat4 viewMatrix;
    mat4 modelMatrix;
    mat3 normalMatrix;
} uniformBufferObject;

layout(location = 0) in vec3 positionAttribute;
layout(location = 1) in vec3 normalAttribute;

layout(location = 0) out vec3 normal;

void main()
{
    gl_Position = (uniformBufferObject.viewMatrix * uniformBufferObject.modelMatrix * vec4(positionAttribute, 1.0));

    normal = normalize(vec3(vec4((mat3(uniformBufferObject.normalMatrix) * normalAttribute), 0.0)));  // thicker model?
    // normal = (tmpNormalMatrix * normalAttribute);  // thinner model?
}