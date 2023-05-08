#include <glm/glm.hpp>

typedef struct _material
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
} Material;
