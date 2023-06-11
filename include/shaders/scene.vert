#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 projectionMatrix;
    mat4 viewMatrix;
    mat4 modelMatrix;
    mat4 normalMatrix;

    vec3 viewingPosition;
    vec4 ambientLightColor;
    vec3 pointLightPosition;
    vec4 pointLightColor;
} uniformBufferObject;

layout(location = 0) in vec3 positionAttribute;
layout(location = 1) in vec3 normalAttribute;
layout(location = 2) in vec2 UVCoordinatesAttribute;

layout(location = 0) out vec3 fragmentPositionWorldSpace;
layout(location = 1) out vec3 fragmentNormalWorldSpace;
layout(location = 2) out vec2 fragmentUVCoordinates;

void main()
{
    vec4 positionAttributeVec4 = vec4(positionAttribute, 1.0);
    gl_Position = (uniformBufferObject.projectionMatrix * uniformBufferObject.viewMatrix * uniformBufferObject.modelMatrix * positionAttributeVec4);
    
    vec4 vertexWorldSpacePosition = (uniformBufferObject.modelMatrix * positionAttributeVec4);
    fragmentPositionWorldSpace = vertexWorldSpacePosition.xyz;
    fragmentNormalWorldSpace = normalize(vec3(vec4((mat3(uniformBufferObject.normalMatrix) * normalAttribute), 0.0)));
    
    fragmentUVCoordinates = UVCoordinatesAttribute;
}