#version 330 core

out vec4 frag_color;

in vec3 v_fragment_position;
in vec3 v_normal;

uniform vec3 u_object_color;
uniform vec3 u_light_color;
uniform vec3 u_light_position;
uniform vec3 u_view_position;

void main()
{
    vec3 normal = normalize(v_normal);
    vec3 light_direction = normalize(u_light_position - v_fragment_position);
    float diff = max(dot(normal, light_direction), 0.0);
    vec3 diffuse_light = diff * u_light_color;

    float ambient_strength = 0.1;
    vec3 ambient_light = ambient_strength * u_light_color;

    vec3 result = (ambient_light + diffuse_light) * u_object_color;
    frag_color = vec4(result, 1.0);
}