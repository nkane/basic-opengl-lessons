#include "glm/glm.hpp"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 texture_coordinates;
};

struct Texture
{
    unsigned int id;
    const char *type;
};

struct Mesh
{

};