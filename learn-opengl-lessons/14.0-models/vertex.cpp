#include "glm/glm.hpp"

typedef struct _vertex 
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec2 texture_coordinates;
} Vertex;
