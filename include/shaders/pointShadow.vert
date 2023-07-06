#version 450

layout(binding = 0) uniform PointShadowUniformBufferObject {
    mat4 projectionMatrix;
    mat4 modelMatrix;
    
    vec3 pointLightPosition;
    uint farPlane;
} uniformBufferObject;

layout(push_constant) uniform PointShadowPushConstants {
    mat4 viewMatrix;
} pushConstants;

layout(location = 0) in vec3 positionAttribute;

layout(location = 0) out VS_OUT {
   vec3 fragmentPositionWorldSpace;
} vsOut;

void main()
{
    gl_Position = (uniformBufferObject.projectionMatrix * pushConstants.viewMatrix  * uniformBufferObject.modelMatrix * vec4(positionAttribute, 1.0));

    vsOut.fragmentPositionWorldSpace = gl_Position.xyz;
}