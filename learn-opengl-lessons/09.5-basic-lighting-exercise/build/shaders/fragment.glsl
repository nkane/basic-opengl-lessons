#version 330 core

out vec4 frag_color;

in vec3 v_lighting_color;

uniform vec3 u_object_color;

void main()
{
    frag_color = vec4(v_lighting_color * u_object_color, 1.0);
}