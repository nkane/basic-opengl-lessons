#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texture_coordinate;

out vec3 v_color;
out vec2 v_texture_coordinate;

void main()
{
   gl_Position = vec4(position, 1.0);
   v_color = color;
   v_texture_coordinate = texture_coordinate;
}