#version 330 core

struct Material 
{
    vec3 ambient_light;
    vec3 diffuse_light;
    vec3 specular_highlight;
    float shininess;
};

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 v_frag_color;

in vec3 v_fragment_position;
in vec3 v_normal;

uniform vec3 u_view_position;
uniform Material u_material;
uniform Light u_light;

void main()
{
    // ambient
    vec3 ambient = u_light.ambient * u_material.ambient_light;

    // diffuse
    vec3 norm = normalize(v_normal);
    vec3 light_directoin = normalize(u_light.position - v_fragment_position);
    float diff = max(dot(norm, light_directoin), 0.0);
    vec3 diffuse = u_light.diffuse * (diff * u_material.diffuse_light);

    // specular
    vec3 view_direction = normalize(u_view_position - v_fragment_position);
    vec3 reflection_direction = reflect(-light_directoin, norm);
    float spec = pow(max(dot(view_direction, reflection_direction), 0.0), u_material.shininess);
    vec3 specular = u_light.specular * (spec * u_material.specular_highlight);

    vec3 result = ambient + diffuse + specular;
    v_frag_color = vec4(result, 1.0);
}