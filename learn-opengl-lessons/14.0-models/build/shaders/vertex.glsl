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
   v_fragment_position = vec3(u_model * vec4(position, 1.0));
   v_normal = mat3(transpose(inverse(u_model))) * normal;
   v_texture_coordinates = texture_coordinates;
   gl_Position = u_projection * u_view * vec4(v_fragment_position, 1.0);
}
