$vertex
#version 330 core

layout (location = 0) in float value;
layout (location = 1) in float day;


void main()
{
    gl_Position = vec4(day,value,0.0,1.0);
}

$fragment
#version 330 core

out vec4 color;

void main()
{
    color = vec4(1.0, 1.0, 1.0, 0.5);
}