#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 projectionMatrix;
    mat4 viewMatrix;
    mat4 modelMatrix;
    mat3 normalMatrix;
} uniformBufferObject;

layout(triangles) in;
layout(line_strip, max_vertices=6) out;

layout(location = 0) in vec3 vsNormals[];

const float MAGNITUDE = 0.2;

void main()
{
    for (int i = 0; i < gl_in.length(); i += 1) {
        gl_Position = (uniformBufferObject.projectionMatrix * gl_in[i].gl_Position);  // "base" vertex.
        EmitVertex();
        gl_Position = (uniformBufferObject.projectionMatrix * (gl_in[i].gl_Position + (vec4(vsNormals[i], 1.0) * MAGNITUDE)));  // "normal-shifted" vertex.
        EmitVertex();

        EndPrimitive();
    }
}