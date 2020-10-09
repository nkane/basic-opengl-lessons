#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec3 v_color;

void main()
{
   gl_Position = vec4(position.x, position.y, position.z, 1.0);
   v_color = vec3(position.r, position.g, position.b);
}