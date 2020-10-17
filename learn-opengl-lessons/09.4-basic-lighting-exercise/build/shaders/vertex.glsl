#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 v_fragment_position;
out vec3 v_normal;
out vec3 v_light_position;

uniform vec3 u_light_position;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
   gl_Position = u_projection * u_view * u_model * vec4(position, 1.0);
   v_fragment_position = vec3(u_view * u_model * vec4(position, 1.0));
   v_normal = mat3(transpose(inverse(u_view * u_model))) * normal;
   // transform world-space light position to view-space ligth position
   v_light_position = vec3(u_view * vec4(u_light_position, 1.0));
}