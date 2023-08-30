#version 450

struct SceneLight {
    uint lightID;
    vec4 lightProperties;
    vec4 lightColor;
};

layout(binding = 0) uniform UniformBufferObject {
    mat4 projectionMatrix;
    mat4 viewMatrix;
    mat4 modelMatrix;
    mat4 normalMatrix;
    mat4 lightSpaceMatrix;

    vec3 viewingPosition;
    
    vec4 ambientLightColor;

    SceneLight sceneLights[10];
    uint sceneLightCount;

    uint farPlane;
} uniformBufferObject;

// TODO: can we make this a struct(VS_IN)?
layout(location = 0) in vec3 positionAttribute;
layout(location = 1) in vec3 normalAttribute;
layout(location = 2) in vec3 tangentAttribute;
layout(location = 3) in vec2 UVCoordinatesAttribute;

layout(location = 0) out VS_OUT {
   vec3 fragmentPositionTangentSpace;
   vec3 viewingPositionTangentSpace;   
   vec3 sceneLightPositionsTangentSpace[10];

   vec4 fragmentPositionLightSpace;
   vec3 fragmentNormalWorldSpace;
   vec2 fragmentUVCoordinates;
} vsOut;

void main()
{
    vec3 calculatedNormal = normalize(vec3(uniformBufferObject.modelMatrix * vec4(normalAttribute, 0.0)));
    vec3 calculatedTangent = normalize(vec3(uniformBufferObject.modelMatrix * vec4(tangentAttribute, 0.0)));
    vec3 calculatedBitangent = cross(calculatedNormal, calculatedTangent);
    mat3 inversedTBNMatrix = transpose(mat3(calculatedTangent, calculatedBitangent, calculatedNormal));

    vec4 positionAttributeVec4 = vec4(positionAttribute, 1.0);
    gl_Position = (uniformBufferObject.projectionMatrix * uniformBufferObject.viewMatrix * uniformBufferObject.modelMatrix * positionAttributeVec4);

    vec4 vertexWorldSpacePosition = (uniformBufferObject.modelMatrix * positionAttributeVec4);
    vsOut.fragmentPositionTangentSpace = (inversedTBNMatrix * vertexWorldSpacePosition.xyz);
    vsOut.viewingPositionTangentSpace = (inversedTBNMatrix * uniformBufferObject.viewingPosition);
    
    for (int i = 0; i < uniformBufferObject.sceneLightCount; ++i) {
        vsOut.sceneLightPositionsTangentSpace[i] = (inversedTBNMatrix * uniformBufferObject.sceneLights[i].lightProperties.xyz);
    }
    
    vsOut.fragmentPositionLightSpace = (uniformBufferObject.lightSpaceMatrix * positionAttributeVec4);
    vsOut.fragmentNormalWorldSpace = normalize(vec3(vec4((mat3(uniformBufferObject.normalMatrix) * normalAttribute), 0.0)));
    
    vsOut.fragmentUVCoordinates = UVCoordinatesAttribute;
}