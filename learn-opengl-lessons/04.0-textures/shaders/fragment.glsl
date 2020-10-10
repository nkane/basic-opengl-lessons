#version 330 core

out vec4 frag_color;

in vec3 v_color;
in vec2 v_texture_coordinate;

uniform sampler2D u_texture;

void main()
{
    frag_color = texture(u_texture, v_texture_coordinate);
}