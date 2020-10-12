#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture_coordinate;

out vec2 v_texture_coordinate;

uniform mat4 u_transform;

void main()
{
   gl_Position = u_transform * vec4(position, 1.0);
   v_texture_coordinate = texture_coordinate;
}