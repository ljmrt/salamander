#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

layout(binding = 0) uniform PointShadowUniformBufferObject {
    glm::mat4 shadowTransforms[6];
    glm::mat4 modelMatrix;
} uniformBufferObject;

layout(location = 0) in vec3 positionAttribute;

void main()
{
    for (int i = 0; i < 6; i++) {  // for each face in a cubemap, essentially.
        gl_Layer = i;  // select the correct depth map layer.
        
        for (int j = 0; j < 3; j++) {  // for each vertice in an triangle.
            gl_Position = (shadowTransforms[i] * gl_in[j].gl_Position);
            EmitVertex();
        }
        EndPrimitive();
    }
}