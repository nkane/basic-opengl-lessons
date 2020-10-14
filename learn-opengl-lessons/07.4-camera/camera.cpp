#define GLEW_STATIC
#include <GL\glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
};

const float YAW             = -90.0f;
const float PITCH           =   0.0f;
const float SPEED           =   2.5f;
const float SENSITIVITY     =   0.1f;
const float ZOOM            =  45.0f;

struct Camera
{
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 world_up;
    float yaw;
    float pitch;
    float movement_speed;
    float mouse_sensitivity;
    float zoom;
};

Camera *
CreateCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
{
    Camera *Result = (Camera *)malloc(sizeof(Camera));
    if (!Result)
    {
        return NULL;
    }
    // set defaults
    Result->position = glm::vec3(0.0f, 0.0f, 0.0f);
    Result->up = glm::vec3(0.0f, 0.0f, 0.0f);
    Result->yaw = YAW;
    Result->pitch = PITCH;
    return Result;
}