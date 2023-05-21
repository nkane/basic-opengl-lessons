#include "glm/fwd.hpp"
#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
};

const float YAW         = -90.0f;
const float PITCH       =   0.0f;
const float SPEED       =   5.0f;
const float SENSITIVITY =   0.1f;
const float ZOOM        =  45.0f;

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

glm::mat4
LookAt(glm::vec3 position, glm::vec3 target, glm::vec3 world_up)
{
    glm::vec3 z_axis = glm::normalize(position - target);
    glm::vec3 x_axis = glm::normalize(glm::cross(glm::normalize(world_up), z_axis));
    glm::vec3 y_axis = glm::cross(z_axis, x_axis);
    glm::mat4 translation = glm::mat4(1.0f);
    translation[3][0] = -position.x;
    translation[3][1] = -position.y;
    translation[3][2] = -position.z;
    glm::mat4 rotation = glm::mat4(1.0f);
    rotation[0][0] = x_axis.x;
    rotation[1][0] = x_axis.y;
    rotation[2][0] = x_axis.z;

    rotation[0][1] = x_axis.x;
    rotation[1][1] = x_axis.y;
    rotation[2][1] = x_axis.z;

    rotation[0][2] = x_axis.x;
    rotation[1][2] = x_axis.y;
    rotation[2][2] = x_axis.z;
    return rotation * translation;
}

void
UpdateCameraVectors(Camera *c)
{
    glm::vec3 front;
    front.x = cos(glm::radians(c->yaw)) * cos(glm::radians(c->pitch));
    front.y = sin(glm::radians(c->pitch));
    front.z = sin(glm::radians(c->yaw)) * cos(glm::radians(c->pitch));
    c->front = glm::normalize(front);
    c->right = glm::normalize(glm::cross(c->front, c->world_up));
    c->up = glm::normalize(glm::cross(c->right, c->front));
}

Camera *
CreateCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
{
    Camera *camera = (Camera *)malloc(sizeof(Camera));
    if (!camera)
    {
        return NULL;
    }
    memset(camera, 0, sizeof(Camera));
    camera->position = position;
    camera->world_up = up;
    camera->yaw = yaw;
    camera->pitch = pitch;
    camera->movement_speed = SPEED;
    camera->mouse_sensitivity = SENSITIVITY;
    camera->zoom = ZOOM;
    UpdateCameraVectors(camera);
    return camera;
}

glm::mat4
GetViewMatrix(Camera *c)
{
    return glm::lookAt(c->position, c->position + c->front, c->up);
}

glm::mat4
CustomGetViewMatrix(Camera *c)
{
#if 0
    const float radius = 10.0f;
    float cam_x = sin(glfwGetTime()) * radius;
    float cam_z = cos(glfwGetTime()) * radius;
    return LookAt(glm::vec3(cam_x, 0.0f, cam_z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
#endif
    return LookAt(c->position, c->position + c->front, c->up);
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
ProcessMouseMovement(Camera *c, float x_offset, float y_offset, GLboolean constraint_pitch)
{
    x_offset *= c->mouse_sensitivity;
    y_offset *= c->mouse_sensitivity;
    c->yaw += x_offset;
    c->pitch += y_offset;
    if (constraint_pitch)
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
    if (c->zoom > 45.0)
    {
        c->zoom = 45.0f;
    }
}
