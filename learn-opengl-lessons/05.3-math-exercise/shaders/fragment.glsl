#version 330 core

out vec4 frag_color;

in vec3 v_color;
in vec2 v_texture_coordinate;

uniform sampler2D u_texture_1;
uniform sampler2D u_texture_2;

void main()
{
    frag_color = mix(texture(u_texture_1, v_texture_coordinate), texture(u_texture_2, v_texture_coordinate), 0.2);
}