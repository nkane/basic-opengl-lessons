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
    vec3 diffuse_light = 0.5 * diff * u_light_color;

    float specular_strength = 0.8;
    vec3 view_direction = normalize(u_view_position - v_fragment_position);
    vec3 reflect_direction = reflect(-light_direction, normal);
    float specular = pow(max(dot(view_direction, reflect_direction), 0.0), 32);
    vec3 specular_highlight = specular_strength * specular * u_light_color;

    float ambient_strength = 0.25;
    vec3 ambient_light = ambient_strength * u_light_color;

    vec3 result = (ambient_light + diffuse_light + specular_highlight) * u_object_color;
    frag_color = vec4(result, 1.0);
}