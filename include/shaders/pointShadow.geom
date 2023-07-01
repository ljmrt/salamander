#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

layout(binding = 0) uniform PointShadowUniformBufferObject {
    mat4 shadowTransforms[6];
    mat4 modelMatrix;
} uniformBufferObject;

void main()
{
    for (int i = 0; i < 6; i++) {  // for each face in a cubemap, essentially.
        gl_Layer = i;  // select the correct depth map layer.
        
        for (int j = 0; j < gl_in.length(); j++) {  // for each vertice in the in structure.
            gl_Position = (uniformBufferObject.shadowTransforms[i] * gl_in[j].gl_Position);
            EmitVertex();
        }
        EndPrimitive();
    }
}