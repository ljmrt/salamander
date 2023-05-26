#version 450
#extension GL_EXT_debug_printf : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 pvMatrix;
    mat4 modelMatrix;
    mat3 normalMatrix;

    vec4 ambientLightColor;
    vec3 pointLightPosition;
    vec4 pointLightColor;
} uniformBufferObject;

layout(binding = 1) uniform sampler2D textureSampler;

layout(location = 0) in vec3 fragmentPositionWorldSpace;
layout(location = 1) in vec3 fragmentNormalWorldSpace;
layout(location = 2) in vec2 fragmentUVCoordinates;

layout(location = 0) out vec4 outputColor;

void main()
{
    vec3 ambientLighting = (uniformBufferObject.ambientLightColor.xyz * uniformBufferObject.ambientLightColor.w);

    vec3 pointLightRayDirection = (uniformBufferObject.pointLightPosition - fragmentPositionWorldSpace);
    float attenuation = (1.0 / dot(pointLightRayDirection, pointLightRayDirection));
    pointLightRayDirection = normalize(pointLightRayDirection);
    
    vec3 diffuseLightColor = (uniformBufferObject.pointLightColor.xyz * uniformBufferObject.pointLightColor.w * attenuation);
    float diffuseLightValue = max(dot(normalize(fragmentNormalWorldSpace), pointLightRayDirection), 0.0);  // we want to avoid negative values.
    vec3 diffuseLighting = (diffuseLightColor * diffuseLightValue);
    
    vec4 completeLighting = vec4((diffuseLighting + ambientLighting), 1.0);
    debugPrintfEXT("Fragment shader printf");
    

    outputColor = (completeLighting * texture(textureSampler, fragmentUVCoordinates));
}