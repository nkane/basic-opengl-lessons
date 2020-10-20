/*
 * Exercise 2
 * - Can you simulate some of the real-world objects by defining their respective materials like we've seen at the start of this chapter?
 *   Note that the table's ambient values are not the same as the diffuse values; they didn't take light intensities into account.
 *   To correctly set their values you'd have to set all the light intensities to vec3(1.0) to get the same output: solution of cyan plastic container.
 */

#include <stdio.h>
#include <memory.h>
#include <math.h>

#include "shader.cpp"
#include "camera.cpp"
#include "material.cpp"

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb\stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static unsigned int WireFrameEnabled = 0;
static unsigned int width = 800;
static unsigned int height = 600;

static Camera *camera = NULL;
static glm::vec3 camera_position   = glm::vec3(0.0f, 0.0f, 25.0f);
static glm::vec3 camera_up         = glm::vec3(0.0f, 1.0f, 0.0f);
static float delta_time = 0.0f;
static float last_frame = 0.0f;

static float yaw = -90.0f;
static float pitch = 0.0f;
static glm::vec3 direction;

void
framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void
process_input(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) ==  GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        WireFrameEnabled = ~WireFrameEnabled;
    }

    const float camera_speed = 2.5f * delta_time;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        ProcessKeyboard(camera, FORWARD, delta_time);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        ProcessKeyboard(camera, BACKWARD, delta_time);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        ProcessKeyboard(camera, LEFT, delta_time);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        ProcessKeyboard(camera, RIGHT, delta_time);
    }
}

void
mouse_callback(GLFWwindow *window, double x, double y)
{
    static bool first_mouse = true;
    static float last_x = 400.0f;
    static float last_y = 300.0f;
    if (first_mouse)
    {
        last_x = x;
        last_y = y;
        first_mouse = false;
        return;
    }
    float x_offset = x - last_x;
    float y_offset = last_y - y;
    last_x = x;
    last_y = y;
    ProcessMouseMovement(camera, x_offset, y_offset, true);
}

void
scroll_callback(GLFWwindow *window, double x_offset, double y_offset)
{
    ProcessMouseScroll(camera, (float)y_offset);
}

int
main()
{
    GLFWwindow *window;
    if (!glfwInit())
    {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (glewInit() != GLEW_OK)
    {
        printf("[GLEW ERROR]: failed to init glew\n");
    }
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // bind vertex array
    unsigned int vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    // copy vertices array in a vertex buffer
    float vertices[] =
    {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    unsigned vertex_buffer_id;
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // set vertex arrtribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int vertex_array_light_id;
    glGenVertexArrays(1, &vertex_array_light_id);
    glBindVertexArray(vertex_array_light_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    ShaderProgram *shader_program = CreateShaderProgram("shaders/vertex.glsl", "shaders/fragment.glsl");   

    ShaderProgram *light_shader_program = CreateShaderProgram("shaders/vertex_light_source.glsl", "shaders/fragment_light_source.glsl");

    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);

    glm::vec3 light_position(0.0f, 2.0f, 1.5f);
    camera = CreateCamera(camera_position, camera_up, yaw, pitch);

    glm::vec3 cube_positions[] =
    {
        glm::vec3(-10.0f, 0.0f, 0.0f),
        glm::vec3(-8.0f, 0.0f, 0.0f),
        glm::vec3(-6.0f, 0.0f, 0.0f),
        glm::vec3(-4.0f, 0.0f, 0.0f),
        glm::vec3(-2.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 0.0f, 0.0f),
        glm::vec3(4.0f, 0.0f, 0.0f),
        glm::vec3(6.0f, 0.0f, 0.0f),
        glm::vec3(8.0f, 0.0f, 0.0f),
        glm::vec3(10.0f, 0.0f, 0.0f),
        glm::vec3(12.0f, 0.0f, 0.0f),

        glm::vec3(-10.0f, -2.0f, 0.0f),
        glm::vec3(-8.0f, -2.0f, 0.0f),
        glm::vec3(-6.0f, -2.0f, 0.0f),
        glm::vec3(-4.0f, -2.0f, 0.0f),
        glm::vec3(-2.0f, -2.0f, 0.0f),
        glm::vec3(0.0f, -2.0f, 0.0f),
        glm::vec3(2.0f, -2.0f, 0.0f),
        glm::vec3(4.0f, -2.0f, 0.0f),
        glm::vec3(6.0f, -2.0f, 0.0f),
        glm::vec3(8.0f, -2.0f, 0.0f),
        glm::vec3(10.0f, -2.0f, 0.0f),
        glm::vec3(12.0f, -2.0f, 0.0f),
    };

    Material materials[] = 
    {
        Material 
        {
            glm::vec3(0.0215f, 0.1745f, 0.0215f),
            glm::vec3(0.07568f,	0.61424f, 0.07568f),
            glm::vec3(0.633f, 0.727811f, 0.633f),
            32.0f,
        },
    };

    while (!glfwWindowShouldClose(window))
    {
        process_input(window);
        {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (WireFrameEnabled)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            glm::mat4 view = glm::mat4(1.0f);
            view = CustomGetViewMatrix(camera);
            glm::mat4 perspective_projection = glm::perspective(glm::radians(camera->zoom), (float)width / (float)height, 0.1f, 100.0f);
            glm::mat4 model = glm::mat4(1.0f);

            // render light source
            glUseProgram(light_shader_program->id);
            glBindVertexArray(vertex_array_light_id);
            model = glm::mat4(1.0f);
            model = glm::translate(model, light_position);
            model = glm::scale(model, glm::vec3(0.2f));
            SetFloatMat4Uniform(light_shader_program, "u_model", glm::value_ptr(model));
            SetFloatMat4Uniform(light_shader_program, "u_view", glm::value_ptr(view));
            SetFloatMat4Uniform(light_shader_program, "u_projection", glm::value_ptr(perspective_projection));
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // render cubes
            glUseProgram(shader_program->id);
            glBindVertexArray(vertex_array_id);
            for (int i = 0; i < sizeof(cube_positions) / sizeof(glm::vec3); i++) 
            {
                SetFloatVec3Uniform(shader_program, "u_material.ambient_light", materials[0].ambient);
                SetFloatVec3Uniform(shader_program, "u_material.diffuse_light", materials[0].diffuse);
                SetFloatVec3Uniform(shader_program, "u_material.specular_highlight", materials[0].specular);
                SetFloatUniform(shader_program, "u_material.shininess", materials[0].shininess);

                model = glm::mat4(1.0f);
                model = glm::translate(model, cube_positions[i]);

                SetFloatMat4Uniform(shader_program, "u_model", glm::value_ptr(model));
                SetFloatMat4Uniform(shader_program, "u_view", glm::value_ptr(view));
                SetFloatMat4Uniform(shader_program, "u_projection", glm::value_ptr(perspective_projection));
                SetFloatVec3Uniform(shader_program, "u_view_position", camera->position);
                SetFloatVec3Uniform(shader_program, "u_light.position", light_position);
                SetFloatVec3Uniform(shader_program, "u_light.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
                SetFloatVec3Uniform(shader_program, "u_light.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
                SetFloatVec3Uniform(shader_program, "u_light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            float current_frame = glfwGetTime();
            delta_time = current_frame - last_frame;
            last_frame = current_frame;
        }
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
    {
        ReleaseShaderProgram(shader_program);
        ReleaseShaderProgram(light_shader_program);
        glDeleteVertexArrays(1, &vertex_array_id);
        glDeleteBuffers(1, &vertex_buffer_id);
        glfwTerminate();
    }
    
    return 0;
}