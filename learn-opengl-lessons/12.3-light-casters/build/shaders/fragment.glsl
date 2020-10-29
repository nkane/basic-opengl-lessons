#version 330 core

struct Material 
{
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
};

struct Light
{
    vec3    position;
    vec3    direction;
    vec3    ambient;
    vec3    diffuse;
    vec3    specular;
    float   cut_off;
    float   outer_cut_off;
    float   constant;
    float   linear;
    float   quadratic;
};

out vec4 v_frag_color;

in vec3 v_fragment_position;
in vec3 v_normal;
in vec2 v_texture_coordinates;

uniform vec3 u_view_position;
uniform Material u_material;
uniform Light u_light;

void main()
{
    // ambient
    vec3 ambient = u_light.ambient * texture(u_material.diffuse, v_texture_coordinates).rgb;

    // diffuse
    vec3 norm = normalize(v_normal);
    vec3  light_direction = normalize(u_light.position - v_fragment_position);
    float diff = max(dot(norm, light_direction), 0.0);
    vec3 diffuse = u_light.diffuse * diff * texture(u_material.diffuse, v_texture_coordinates).rgb;

    // specular
    vec3 view_direction = normalize(u_view_position - v_fragment_position);
    vec3 reflection_direction = reflect(-light_direction, norm);
    float spec = pow(max(dot(view_direction, reflection_direction), 0.0), u_material.shininess);
    vec3 specular = u_light.specular * spec * texture(u_material.specular, v_texture_coordinates).rgb;


    // spotlight (soft edges)
    float theta           = dot(light_direction, normalize(-u_light.direction));
    float epsilon         = u_light.cut_off - u_light.outer_cut_off;
    float intensity       = clamp((theta - u_light.outer_cut_off) / epsilon, 0.0, 1.0);
    diffuse *= intensity;
    specular *= intensity;

    // attenuation
    float distance    = length(u_light.position - v_fragment_position);
    float attenuation = 1.0 / (u_light.constant + u_light.linear * distance + u_light.quadratic * (distance * distance));    
    ambient  *= attenuation;
    diffuse   *= attenuation;
    specular *= attenuation;   

    vec3 result = ambient + diffuse + specular;
    v_frag_color = vec4(result, 1.0);
}