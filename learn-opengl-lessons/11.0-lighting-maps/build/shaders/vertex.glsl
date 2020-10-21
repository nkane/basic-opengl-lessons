#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coordinates;

out vec3 v_fragment_position;
out vec3 v_normal;
out vec2 v_texture_coordinates;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
   gl_Position = u_projection * u_view * u_model * vec4(position, 1.0);
   v_normal = normal;
   v_fragment_position = vec3(u_model * vec4(position, 1.0));
   v_texture_coordinates = texture_coordinates;
}