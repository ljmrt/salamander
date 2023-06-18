#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 projectionMatrix;
    mat4 viewMatrix;
    mat4 modelMatrix;
    mat4 normalMatrix;
    mat4 lightSpaceMatrix;

    vec3 viewingPosition;
    vec4 ambientLightColor;
    vec3 pointLightPosition;
    vec4 pointLightColor;
} uniformBufferObject;

layout(binding = 1) uniform sampler2D textureSampler;
layout(binding = 2) uniform sampler2D shadowMapSampler;

layout(location = 0) in VS_OUT {
   vec3 fragmentPositionWorldSpace;
   vec4 fragmentPositionLightSpace;
   vec3 fragmentNormalWorldSpace;
   vec2 fragmentUVCoordinates;
} vsOut;

layout(location = 0) out vec4 outputColor;

void main()
{
    vec3 ambientLighting = (uniformBufferObject.ambientLightColor.xyz * uniformBufferObject.ambientLightColor.w);

    vec3 pointLightRayDirection = (uniformBufferObject.pointLightPosition - vsOut.fragmentPositionWorldSpace);
    float attenuation = (1.0 / dot(pointLightRayDirection, pointLightRayDirection));
    pointLightRayDirection = normalize(pointLightRayDirection);
    
    vec3 unpackedPointLightColor = (uniformBufferObject.pointLightColor.xyz * uniformBufferObject.pointLightColor.w * attenuation);
    float diffuseLightValue = max(dot(normalize(vsOut.fragmentNormalWorldSpace), pointLightRayDirection), 0.0);  // we want to avoid negative values.
    vec3 diffuseLighting = (unpackedPointLightColor * diffuseLightValue);

    float shininessValue = 16;
    float specularExponent = 1;
    
    vec3 viewingDirection = normalize(uniformBufferObject.viewingPosition - vsOut.fragmentPositionWorldSpace);
    vec3 reflectionDirection = reflect(pointLightRayDirection, vsOut.fragmentNormalWorldSpace);
    vec3 halfwayDirection = normalize(pointLightRayDirection + viewingDirection);
    float specularComponent = pow(max(dot(vsOut.fragmentNormalWorldSpace, halfwayDirection), 0.0), shininessValue);
    
    vec3 specularLighting = specularExponent * specularComponent * unpackedPointLightColor;

    vec3 projectedCoordinates = (vsOut.fragmentPositionLightSpace.xyz / vsOut.fragmentPositionLightSpace.w);
    projectedCoordinates = ((projectedCoordinates * 0.5) + 0.5);  // transform coordinates from -1..1 to 0..1.
    
    float closestDepthAtCoordinates = texture(shadowMapSampler, projectedCoordinates.xy).r;
    float currentDepthAtCoordinates = projectedCoordinates.z;

    float isObscured = closestDepthAtCoordinates < currentDepthAtCoordinates ? 1.0 : 0.0;

    vec4 completeLighting = vec4((ambientLighting + ((diffuseLighting + specularLighting) * (1.0 - isObscured))), 1.0);


    outputColor = (completeLighting * texture(textureSampler, vsOut.fragmentUVCoordinates));
}