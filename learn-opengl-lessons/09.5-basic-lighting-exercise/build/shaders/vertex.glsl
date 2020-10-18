#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 v_lighting_color;

uniform vec3 u_light_position;
uniform vec3 u_view_position;
uniform vec3 u_light_color;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
   gl_Position = u_projection * u_view * u_model * vec4(position, 1.0);
   // gouraud shading
   vec3 position = vec3(u_model * vec4(position, 1.0));
   vec3 model_normal = mat3(transpose(inverse(u_model))) * normal;

   // ambient
   float ambient_strength = 0.1;
   vec3 ambient_light = ambient_strength * u_light_color;

   // diffuse
   vec3 norm = normalize(model_normal);
   vec3 light_direction = normalize(u_light_position - position);
   float diff = max(dot(norm, light_direction), 0.0);
   vec3 diffuse_light = diff * u_light_color;

   // specular
   float specular_strength = 1.0; // this is set higher to better show the effect of Gouraud shading
   vec3 view_direction = normalize(u_view_position - position);
   vec3 reflection_direction = reflect(-light_direction, norm);
   float spec = pow(max(dot(view_direction, reflection_direction), 0.0), 32);
   vec3 specular_hightlight = specular_strength * spec * u_light_color;

   v_lighting_color = ambient_light + diffuse_light + specular_hightlight;
}