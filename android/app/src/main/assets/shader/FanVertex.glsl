#version 300 es

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexShade;

uniform mat4 MODELVIEWPROJECTIONMATRIX;

out vec3 Shade;

void main()
{
    Shade = VertexShade;
    gl_Position = MODELVIEWPROJECTIONMATRIX * vec4(VertexPosition, 1.0);
}