#version 330 core
out vec4 frag_color;

in vec2 texture_coordinates;

uniform sampler2D texture_diffuse_1;

void main()
{
    frag_color = vec4(1.0, 0.0, 0.0, 1.0);
    //frag_color = texture(texture_diffuse_1, texture_coordinates);
}
