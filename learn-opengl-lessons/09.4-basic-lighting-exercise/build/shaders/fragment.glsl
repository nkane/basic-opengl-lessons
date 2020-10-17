#version 330 core

out vec4 frag_color;

in vec3 v_fragment_position;
in vec3 v_normal;
in vec3 v_light_position;

uniform vec3 u_object_color;
uniform vec3 u_light_color;

void main()
{
    // ambient light
    float ambient_strength = 0.1;
    vec3 ambient_light = ambient_strength * u_light_color;

    // diffuse
    vec3 norm = normalize(v_normal);
    vec3 light_direction = normalize(v_light_position - v_fragment_position);
    float diff = max(dot(norm, light_direction), 0.0);
    vec3 diffuse_light = diff * u_light_color;

    // specular
    float specular_strength = 0.5;
    vec3 view_direction = normalize(-v_fragment_position);
    vec3 reflection_direction = reflect(-light_direction, norm);
    float spec = pow(max(dot(view_direction, reflection_direction), 0.0), 32);
    vec3 specular_highlight = specular_strength * spec * u_light_color;

    vec3 result = (ambient_light + diffuse_light + specular_highlight) * u_object_color;
    frag_color = vec4(result, 1.0);
}