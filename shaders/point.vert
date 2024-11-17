#version 420 core

layout (location = 0) in vec3 pos;

uniform float size;
out vec4 vCol;

void main()
{
    gl_Position = vec4(pos, 1.0);
    gl_PointSize = size;
    vCol = vec4(0.8f, 0.8f, 0.8f, 1.0f);
}
