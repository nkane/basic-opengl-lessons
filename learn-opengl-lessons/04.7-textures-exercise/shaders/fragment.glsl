#version 330 core

out vec4 frag_color;

in vec3 v_color;
in vec2 v_texture_coordinate;

uniform sampler2D u_texture_1;
uniform sampler2D u_texture_2;
uniform float u_mix;

void main()
{
    vec2 temp_position = vec2(1.0 - v_texture_coordinate.x, v_texture_coordinate.y);
    frag_color = mix(texture(u_texture_1, v_texture_coordinate), texture(u_texture_2, temp_position), u_mix);
}