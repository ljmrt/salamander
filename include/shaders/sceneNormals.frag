#version 450

layout(binding = 1) uniform sampler2D textureSampler;  // unused.

layout(location = 0) out vec4 fragmentOutputColor;

void main()
{
    fragmentOutputColor = vec4(1.0, 1.0, 0.0, 1.0);  // yellow.
}