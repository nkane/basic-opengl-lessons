#ifndef _INTERNAL_VERTEX
#define _INTERNAL_VERTEX

#ifndef GLM_DEFINED
#define GLM_DEFINED
#include <glm/glm.hpp>
#endif

typedef struct _vertex 
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec2 texture_coordinates;
    int m_bone_ids[4];
    float m_weights[4];
} Vertex;

#endif
