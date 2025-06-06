// A simple shader that pass the incoming vertex data
// to the rasterizer without any modification.
#version 410 core

layout (location = 0) in vec3 in_Pos;
layout (location = 1) in vec3 in_Color;

out vec3 vertexColor;

void main()
{
    gl_Position = vec4(in_Pos, 1.0);
    vertexColor = in_Color;
}
