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

    SceneLight sceneLights[128];
    uint sceneLightCount;

    uint farPlane;
} uniformBufferObject;

layout(binding = 1) uniform sampler2D textureSampler;
layout(binding = 2) uniform sampler2D directionalShadowSampler;
layout(binding = 3) uniform samplerCube pointShadowSampler;

layout(location = 0) in VS_OUT {
   vec3 fragmentPositionWorldSpace;
   vec4 fragmentPositionLightSpace;
   vec3 fragmentNormalWorldSpace;
   vec2 fragmentUVCoordinates;
} vsOut;

layout(location = 0) out vec4 outputColor;

vec3 calculateSceneLightImpact(SceneLight sceneLight, vec3 fragmentPosition, vec3 fragmentNormal, vec3 viewingDirection);
float calculateDirectionalShadowObscurity(vec4 fragmentPositionLightSpace, float shadowBias);
float calculatePointShadowObscurity(vec3 fragmentPosition, SceneLight sceneLight, float shadowBias);

void main()
{
    outputColor = vec4(0.0, 0.0, 0.0, 1.0);

    vec3 ambientLighting = (uniformBufferObject.ambientLightColor.xyz * uniformBufferObject.ambientLightColor.w);
    outputColor += vec4(ambientLighting, 1.0);

    vec3 viewingDirection = normalize(uniformBufferObject.viewingPosition - vsOut.fragmentPositionWorldSpace);
    for (int i = 0; i < uniformBufferObject.sceneLightCount; i++) {
        outputColor += vec4(calculateSceneLightImpact(uniformBufferObject.sceneLights[i], vsOut.fragmentPositionWorldSpace, vsOut.fragmentNormalWorldSpace, viewingDirection), 0.0);
    }

    outputColor *= texture(textureSampler, vsOut.fragmentUVCoordinates);
}

vec3 calculateSceneLightImpact(SceneLight sceneLight, vec3 fragmentPosition, vec3 fragmentNormal, vec3 viewingDirection)
{
    vec3 lightRayDirection = (sceneLight.lightProperties.xyz - (fragmentPosition * sceneLight.lightProperties.w));  // selectively change the direction if the light is point or directional.

    float distance = length(lightRayDirection);
    float attenuation = (1.0 / (1.0 + (0.09 * distance) + (0.032 * (distance * distance))));  // dot(lightRayDirection, lightRayDirection));
    attenuation = (sceneLight.lightID == 1 ? attenuation : 1);  // selectively disable attenuation depending on the light type.

    lightRayDirection = normalize(lightRayDirection);

    vec3 unpackedLightColor = (sceneLight.lightColor.xyz * sceneLight.lightColor.w * attenuation);  // packed as RGB color, and the intensity as the A-channel.


    float diffuseLightValue = max(dot(normalize(fragmentNormal), lightRayDirection), 0.0);  // we want to avoid negative values.
    vec3 diffuseLighting = (unpackedLightColor * diffuseLightValue);


    float shininessValue = 16;
    float specularExponent = (sceneLight.lightID == 1 ? 4 : 0);  // selectively disable specular lighting depending on the light type.

    vec3 reflectionDirection = reflect(lightRayDirection, fragmentNormal);
    vec3 halfwayDirection = normalize(lightRayDirection + viewingDirection);
    float specularComponent = pow(max(dot(fragmentNormal, halfwayDirection), 0.0), shininessValue);
    
    vec3 specularLighting = specularExponent * specularComponent * unpackedLightColor;

    float shadowBias = max((0.05 * (1.0 - dot(fragmentNormal, lightRayDirection))), 0.005);
    float isObscured = (sceneLight.lightID == 1 ? calculatePointShadowObscurity(fragmentPosition, sceneLight, shadowBias) : calculateDirectionalShadowObscurity(vsOut.fragmentPositionLightSpace, shadowBias));

    return vec3(((diffuseLighting + specularLighting) * (1.0 - isObscured)));
}

float calculateDirectionalShadowObscurity(vec4 fragmentPositionLightSpace, float shadowBias)
{
    vec3 projectedCoordinates = (fragmentPositionLightSpace.xyz / fragmentPositionLightSpace.w);
    projectedCoordinates = ((projectedCoordinates * 0.5) + 0.5);  // transform coordinates from -1..1 to 0..1.

    float sampledDepthAtCoordinates = texture(directionalShadowSampler, projectedCoordinates.xy).r;
    float actualDepthAtCoordinates = projectedCoordinates.z;

    return ((sampledDepthAtCoordinates < actualDepthAtCoordinates - shadowBias) ? 1.0 : 0.0);
}

float calculatePointShadowObscurity(vec3 fragmentPosition, SceneLight sceneLight, float shadowBias)
{
    vec3 shadowSampleDirection = (fragmentPosition - sceneLight.lightProperties.xyz);

    float sampledDepthAtCoordinates = texture(pointShadowSampler, shadowSampleDirection).r;
    sampledDepthAtCoordinates *= uniformBufferObject.farPlane;

    float actualDepthAtCoordinates = length(shadowSampleDirection);
    actualDepthAtCoordinates -= (gl_FrontFacing ? shadowBias : 0.0);

    return ((sampledDepthAtCoordinates < actualDepthAtCoordinates) ? 1.0 : 0.0);
}