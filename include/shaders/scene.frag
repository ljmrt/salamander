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

    vec3 mainLightRayDirection = (uniformBufferObject.mainLightProperties.xyz - (vsOut.fragmentPositionWorldSpace * uniformBufferObject.mainLightProperties.w));  // selectively change the direction if the light is point or directional.

    float attenuation = (1.0 / dot(mainLightRayDirection, mainLightRayDirection));

    attenuation = ((uniformBufferObject.mainLightProperties.w == 0.0) ? 1 : attenuation);  // selectively disable attenuation(based on directional or positional properties). TODO: is this branching?

    mainLightRayDirection = normalize(mainLightRayDirection);
    
    vec3 unpackedMainLightColor = (uniformBufferObject.mainLightColor.xyz * uniformBufferObject.mainLightColor.w * attenuation);
    float diffuseLightValue = max(dot(normalize(vsOut.fragmentNormalWorldSpace), mainLightRayDirection), 0.0);  // we want to avoid negative values.
    vec3 diffuseLighting = (unpackedMainLightColor * diffuseLightValue);

    float shininessValue = 16;
    float specularExponent = (2 * uniformBufferObject.mainLightProperties.w);
    
    vec3 viewingDirection = normalize(uniformBufferObject.viewingPosition - vsOut.fragmentPositionWorldSpace);
    vec3 reflectionDirection = reflect(mainLightRayDirection, vsOut.fragmentNormalWorldSpace);
    vec3 halfwayDirection = normalize(mainLightRayDirection + viewingDirection);
    float specularComponent = pow(max(dot(vsOut.fragmentNormalWorldSpace, halfwayDirection), 0.0), shininessValue);
    
    vec3 specularLighting = specularExponent * specularComponent * unpackedMainLightColor;

    vec3 projectedCoordinates = (vsOut.fragmentPositionLightSpace.xyz / vsOut.fragmentPositionLightSpace.w);
    projectedCoordinates = ((projectedCoordinates * 0.5) + 0.5);  // transform coordinates from -1..1 to 0..1.
    
    float closestDepthAtCoordinates = texture(shadowMapSampler, projectedCoordinates.xy).r;
    float currentDepthAtCoordinates = projectedCoordinates.z;
    
    float shadowBias = max((0.05 * (1.0 - dot(vsOut.fragmentNormalWorldSpace, mainLightRayDirection))), 0.005);
    currentDepthAtCoordinates += (gl_FrontFacing ? shadowBias : 0.0);

    float isObscured = ((closestDepthAtCoordinates < currentDepthAtCoordinates) ? 1.0 : 0.0);

    vec4 completeLighting = vec4((ambientLighting + ((diffuseLighting + specularLighting) * (1.0 - isObscured))), 1.0);


    outputColor = (completeLighting * texture(textureSampler, vsOut.fragmentUVCoordinates));

    // used to debug isObscured, blue if is obscured/in the shadow, yellow if not obscured/being lit.
    // if (isObscured == 0.0) {
    //    outputColor = vec4(1.0, 1.0, 0.0, 1.0);
    // }
    // if (isObscured == 1.0) {
    //    outputColor = vec4(0.0, 0.0, 1.0, 1.0);
    // }
}