#version 420 core

layout (location = 0) in vec3 pos;

uniform float size;
uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

out vec4 vCol;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0);
    gl_PointSize = size;
    vCol = vec4(pos, 1.0f);
}
