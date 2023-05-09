#version 330 core

struct Material 
{
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
};

struct Directional_Light
{
    vec3    direction;
    vec3    ambient;
    vec3    diffuse;
    vec3    specular;
};

struct Point_Light
{
    vec3    position;
    float   constant;
    float   linear;
    float   quadratic;
    vec3    ambient;
    vec3    diffuse;
    vec3    specular;
};

struct Spot_Light
{
    vec3    position;
    vec3    direction;
    float   cut_off;
    float   outer_cut_off;
    float   constant;
    float   linear;
    float   quadratic;
    vec3    ambient;
    vec3    diffuse;
    vec3    specular;
};

#define NR_POINT_LIGHTS 4

in vec3 v_fragment_position;
in vec3 v_normal;
in vec2 v_texture_coordinates;

uniform vec3              u_view_position;
uniform Material          u_material;
uniform Directional_Light u_directional_light;
uniform Spot_Light        u_spot_light;
uniform Point_Light       u_point_lights[NR_POINT_LIGHTS];

out vec4 v_frag_color;

vec3
Calculate_Directional_Light(Directional_Light light, vec3 normal, vec3 view_direction)
{
    vec3 light_direction = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, light_direction), 0.0);
    // specular shading
    vec3 reflection_direction = reflect(-light_direction, normal);
    float spec = pow(max(dot(view_direction, reflection_direction), 0.0), u_material.shininess);
    // combine results
    vec3 ambient  = light.ambient * vec3(texture(u_material.diffuse, v_texture_coordinates));
    vec3 diffuse  = light.diffuse * diff * vec3(texture(u_material.diffuse, v_texture_coordinates));
    vec3 specular = light.specular * spec * vec3(texture(u_material.specular, v_texture_coordinates));
    return ambient + diffuse + specular;
}

vec3
Calculate_Point_Light(Point_Light light, vec3 normal, vec3 fragment_position, vec3 view_direction)
{
    vec3 light_direction = normalize(light.position - fragment_position);
    // diffuse shading
    float diff = max(dot(normal, light_direction), 0.0);
    // specular shading
    vec3 reflection_direction = reflect(-light_direction, normal);
    float spec = pow(max(dot(view_direction, reflection_direction), 0.0), u_material.shininess);
    // attenuation
    float distance      = length(light.position - fragment_position);
    float attenuation   = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient  = light.ambient * vec3(texture(u_material.diffuse, v_texture_coordinates));
    vec3 diffuse  = light.diffuse * diff * vec3(texture(u_material.diffuse, v_texture_coordinates));
    vec3 specular = light.specular * spec * vec3(texture(u_material.specular, v_texture_coordinates));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return ambient + diffuse + specular;
}

vec3
Calculate_Spot_Light(Spot_Light light, vec3 normal, vec3 fragment_position, vec3 view_direction)
{
    vec3 light_direction = normalize(light.position - fragment_position);
    // diffuse shading
    float diff = max(dot(normal, light_direction), 0.0);
    // specular shading
    vec3 reflection_direction = reflect(-light_direction, normal);
    float spec = pow(max(dot(view_direction, reflection_direction), 0.0), u_material.shininess);
    // attenuation
    float distance = length(light.position - fragment_position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(light_direction, normalize(-light.direction)); 
    float epsilon = light.cut_off - light.outer_cut_off;
    float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(u_material.diffuse, v_texture_coordinates));
    vec3 diffuse = light.diffuse * diff * vec3(texture(u_material.diffuse, v_texture_coordinates));
    vec3 specular = light.specular * spec * vec3(texture(u_material.specular, v_texture_coordinates));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return ambient + diffuse + specular;
}

void
main()
{
    // properties
    vec3 norm = normalize(v_normal);
    vec3 view_direction = normalize(u_view_position - v_fragment_position);
    // phase 1: directional lighting
    vec3 result = Calculate_Directional_Light(u_directional_light, norm, view_direction);
    // phase 2: point lights
    for (int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        result += Calculate_Point_Light(u_point_lights[i], norm, v_fragment_position, view_direction);
    }
    // phase 3: spot light
    result += Calculate_Spot_Light(u_spot_light, norm, v_fragment_position, view_direction);
    v_frag_color = vec4(result, 1.0);
}