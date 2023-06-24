#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 projectionMatrix;
    mat4 viewMatrix;
    mat4 modelMatrix;
    mat4 normalMatrix;
    mat4 lightSpaceMatrix;

    vec3 viewingPosition;
    
    vec4 ambientLightColor;

    vec4 mainLightProperties;
    vec4 mainLightColor;
} uniformBufferObject;

// TODO: can we make this a struct(VS_IN)?
layout(location = 0) in vec3 positionAttribute;
layout(location = 1) in vec3 normalAttribute;
layout(location = 2) in vec2 UVCoordinatesAttribute;

layout(location = 0) out VS_OUT {
   vec3 fragmentPositionWorldSpace;
   vec4 fragmentPositionLightSpace;
   vec3 fragmentNormalWorldSpace;
   vec2 fragmentUVCoordinates;
} vsOut;

void main()
{
    vec4 positionAttributeVec4 = vec4(positionAttribute, 1.0);
    gl_Position = (uniformBufferObject.projectionMatrix * uniformBufferObject.viewMatrix * uniformBufferObject.modelMatrix * positionAttributeVec4);
    
    vec4 vertexWorldSpacePosition = (uniformBufferObject.modelMatrix * positionAttributeVec4);
    vsOut.fragmentPositionWorldSpace = vertexWorldSpacePosition.xyz;
    vsOut.fragmentPositionLightSpace = (uniformBufferObject.lightSpaceMatrix * positionAttributeVec4);
    vsOut.fragmentNormalWorldSpace = normalize(vec3(vec4((mat3(uniformBufferObject.normalMatrix) * normalAttribute), 0.0)));
    
    vsOut.fragmentUVCoordinates = UVCoordinatesAttribute;
}