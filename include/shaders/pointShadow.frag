#version 450

layout(binding = 0) uniform PointShadowUniformBufferObject {
    mat4 projectionMatrix;
    mat4 modelMatrix;
    
    vec3 pointLightPosition;
    uint farPlane;
} uniformBufferObject;

layout(location = 0) in VS_OUT {
   vec3 fragmentPositionWorldSpace;
} vsOut;

void main()
{
    float distanceToLight = length(vsOut.fragmentPositionWorldSpace - uniformBufferObject.pointLightPosition);
    distanceToLight /= uniformBufferObject.farPlane;

    gl_FragDepth = distanceToLight;
}