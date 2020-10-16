#version 330 core

out vec4 frag_color;

uniform vec3 u_object_color;
uniform vec3 u_light_color;

void main()
{
    float ambient_strength = 0.1;
    vec3 ambient_light = ambient_strength * u_light_color;
    frag_color = vec4(ambient_light * u_object_color, 1.0);
}