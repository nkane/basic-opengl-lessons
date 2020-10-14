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

void
UpdateCameraVectors(Camera *c)
{
    glm::vec3 front;
    front.x = cos(glm::radians(c->yaw)) * cos(glm::radians(c->pitch));
    front.y = sin(glm::radians(c->pitch));
    front.z = sin(glm::radians(c->yaw)) * cos(glm::radians(c->pitch));
    c->front = glm::normalize(front);
    c->right = glm::normalize(glm::cross(c->front, c->world_up));
    c->up    = glm::normalize(glm::cross(c->right, c->front));
}

Camera *
CreateCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
{
    Camera *Result = (Camera *)malloc(sizeof(Camera));
    if (!Result)
    {
        return NULL;
    }
    //memset(Result, 0, sizeof(Camera));
    Result->position = position;
    Result->world_up = up;
    Result->yaw = yaw;
    Result->pitch = pitch;
    Result->movement_speed = SPEED;
    Result->mouse_sensitivity = SENSITIVITY;
    Result->zoom = ZOOM;
    UpdateCameraVectors(Result);
    return Result;
}

glm::mat4
GetViewMatrix(Camera *c)
{
    return glm::lookAt(c->position, c->position + c->front, c->up);
}

void
ProcessKeyboard(Camera *c, Camera_Movement direction, float delta_time)
{
    float velocity = c->movement_speed * delta_time;
    switch (direction)
    {
        case FORWARD:
        {
            c->position += c->front * velocity;
        } break;

        case BACKWARD:
        {
            c->position -= c->front * velocity;
        } break;

        case LEFT:
        {
            c->position -= c->right * velocity;
        } break;

        case RIGHT:
        {
            c->position += c->right * velocity;
        } break;
    }
}

void
ProcessMouseMovement(Camera *c, float x_offset, float y_offset, GLboolean constraintPitch)
{
    x_offset *= c->mouse_sensitivity;
    y_offset *= c->mouse_sensitivity;
    c->yaw += x_offset;
    c->pitch += y_offset;
    if (constraintPitch)
    {
        if (c->pitch > 89.0f)
        {
            c->pitch = 89.0f;
        }
        if (c->pitch < -89.0f)
        {
            c->pitch = -89.0f;
        }
    }
    UpdateCameraVectors(c);
}

void
ProcessMouseScroll(Camera *c, float y_offset)
{
    c->zoom -= y_offset;
    if (c->zoom < 1.0f)
    {
        c->zoom = 1.0f;
    }
    if (c->zoom > 45.0f)
    {
        c->zoom = 45.0f;
    }
}