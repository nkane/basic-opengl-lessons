#include <cstddef>
#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <vector>
#include <map>

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb\stb_image.h>

#define GLM_DEFINED 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.cpp"
#include "camera.cpp"
#include "texture.cpp"

static unsigned int wire_frame_enabled = 0;
static unsigned int screen_width = 800;
static unsigned int screen_height = 600;

static Camera *camera = NULL;
static glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 3.0f);
static glm::vec3 camera_up       = glm::vec3(0.0f, 1.0f, 0.0f);

static float delta_time = 0.0f;
static float last_frame = 0.0f;

static float yaw = -90.0f;
static float pitch = 0.0f;
static glm::vec3 direction;

void
FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void
ProcessInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        wire_frame_enabled = ~wire_frame_enabled;
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
MouseCallback(GLFWwindow *window, double x, double y)
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
ScrollCallback(GLFWwindow *window, double x_offset, double y_offset)
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
    glfwWindowHint(GLFW_OPENGL_PROFILE,  GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(screen_width, screen_height, "LearnOpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);

    if (glewInit() != GLEW_OK)
    {
        printf("[GLEW ERROR]: failed to init glew\n");
        glfwTerminate();
        return -1;
    }

    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

    glViewport(0, 0, screen_width, screen_height);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ShaderProgram *shader = CreateShaderProgram("./shaders/vertex.glsl", "./shaders/fragment.glsl");
    if (!shader)
    {
        glfwTerminate();
        return -1;
    }

    float cube_vertices[] = 
    {
        // positions          // texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    float plane_vertices[] = 
    {
        // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };

    float transparent_vertices[] = {
        // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
        1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };

    // cube vertex array object
    unsigned int cube_vertex_array_object;
    unsigned int cube_vertex_buffer_object; 
    glGenVertexArrays(1, &cube_vertex_array_object);
    glGenBuffers(1, &cube_vertex_buffer_object);
    glBindVertexArray(cube_vertex_array_object);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), &cube_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glBindVertexArray(0);

    // plane vertex array object
    unsigned int plane_vertex_array_object;
    unsigned int plane_vertex_buffer_object;
    glGenVertexArrays(1, &plane_vertex_array_object);
    glGenBuffers(1, &plane_vertex_buffer_object);
    glBindVertexArray(plane_vertex_array_object);
    glBindBuffer(GL_ARRAY_BUFFER, plane_vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), &plane_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glBindVertexArray(0);

    // transparent vertex array object
    unsigned int transparent_vertex_array_object;
    unsigned int transparent_vertex_buffer_object;
    glGenVertexArrays(1, &transparent_vertex_array_object);
    glGenBuffers(1, &transparent_vertex_buffer_object);
    glBindVertexArray(transparent_vertex_array_object);
    glBindBuffer(GL_ARRAY_BUFFER, transparent_vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparent_vertices), transparent_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glBindVertexArray(0);

    // texture loading and binding
    Texture marble_texture = {};
    stbi_set_flip_vertically_on_load(true);
    const char *marble_path = "assets/marble.jpg";
    marble_texture.data = stbi_load(marble_path, 
                          &marble_texture.width,
                          &marble_texture.height,
                          &marble_texture.channels, 0);
    if (!marble_texture.data)
    {
        printf("[STB_Image ERROR]: failed to load image (%s)\n", marble_path);
        return -1;
    }
    GLenum format;
    switch (marble_texture.channels)
    {
        case 1:
        {
            format = GL_RED;
        } break;
        case 3:
        {
            format = GL_RGB;
        } break;
        case 4:
        {
            format = GL_RGBA;
        } break;
    }
    glGenTextures(1, &marble_texture.id);
    glBindTexture(GL_TEXTURE_2D, marble_texture.id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, marble_texture.width, marble_texture.height, 0, format, GL_UNSIGNED_BYTE, marble_texture.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(marble_texture.data);

    Texture metal_texture = {};
    const char *metal_path = "assets/metal.png";
    metal_texture.data = stbi_load(metal_path, 
                          &metal_texture.width,
                          &metal_texture.height,
                          &metal_texture.channels, 0);
    if (!metal_texture.data)
    {
        printf("[STB_Image ERROR]: failed to load image (%s)\n", metal_path);
        return -1;
    }
    switch (metal_texture.channels)
    {
        case 1:
        {
            format = GL_RED;
        } break;
        case 3:
        {
            format = GL_RGB;
        } break;
        case 4:
        {
            format = GL_RGBA;
        } break;
    }
    glGenTextures(1, &metal_texture.id);
    glBindTexture(GL_TEXTURE_2D, metal_texture.id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, metal_texture.width, metal_texture.height, 0, format, GL_UNSIGNED_BYTE, metal_texture.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(metal_texture.data);

    Texture transparent_texture = {};
    const char *transparent_texture_path = "assets/window.png";
    stbi_set_flip_vertically_on_load(false);
    transparent_texture.data = stbi_load(transparent_texture_path,
                          &transparent_texture.width,
                          &transparent_texture.height,
                          &transparent_texture.channels, 0);
    if (!transparent_texture.data)
    {
        printf("[STB_Image ERROR]: failed to load image (%s)\n", transparent_texture_path);
        return -1;
    }
    switch (transparent_texture.channels)
    {
        case 1:
        {
            format = GL_RED;
        } break;
        case 3:
        {
            format = GL_RGB;
        } break;
        case 4:
        {
            format = GL_RGBA;
        } break;
    }
    glGenTextures(1, &transparent_texture.id);
    glBindTexture(GL_TEXTURE_2D, transparent_texture.id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, transparent_texture.width, transparent_texture.height, 0, format, GL_UNSIGNED_BYTE, transparent_texture.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(transparent_texture.data);

    std::vector<glm::vec3> windows;
    windows.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
    windows.push_back(glm::vec3( 1.5f, 0.0f,  0.51f));
    windows.push_back(glm::vec3( 0.0f, 0.0f,  0.70f));
    windows.push_back(glm::vec3(-0.3f, 0.0f, -2.30f));
    windows.push_back(glm::vec3( 0.5f, 0.0f, -0.60f));

    // camera
    camera = CreateCamera(camera_position, camera_up, yaw, pitch);

    glUseProgram(shader->id);
    SetIntUniform(shader, "texutre_1", 0);

    while (!glfwWindowShouldClose(window))
    {
        float current_frame = (float)(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        // input
        ProcessInput(window);

        // sort the transparent windows before rendering
        std::map<float, glm::vec3> sorted;
        for (unsigned int i = 0; i < windows.size(); i++)
        {
            float distance = glm::length(camera->position - windows[i]);
            sorted[distance] = windows[i];
        }

        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader->id);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = GetViewMatrix(camera);
        glm::mat4 projection = glm::perspective(glm::radians(camera->zoom), (float)screen_width / (float)screen_height, 0.1f, 100.0f);
        SetFloatMat4Uniform(shader, "view", glm::value_ptr(view));
        SetFloatMat4Uniform(shader, "projection", glm::value_ptr(projection));

        // cubes
        glBindVertexArray(cube_vertex_array_object);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, marble_texture.id);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        SetFloatMat4Uniform(shader,"model", glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        SetFloatMat4Uniform(shader,"model", glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // floor
        glBindVertexArray(plane_vertex_array_object);
        glBindTexture(GL_TEXTURE_2D, metal_texture.id);
        glm::mat4 floor_model = glm::mat4(1.0f);
        SetFloatMat4Uniform(shader,"model", glm::value_ptr(floor_model));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        // vegetation
        glBindVertexArray(transparent_vertex_array_object);
        glBindTexture(GL_TEXTURE_2D, transparent_texture.id);

        glm::mat4 transparent_model = glm::mat4(1.0f);
        for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
        {
            transparent_model = glm::mat4(1.0f);
            transparent_model = glm::translate(model, it->second);
            SetFloatMat4Uniform(shader, "model", glm::value_ptr(transparent_model));
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &cube_vertex_array_object);
    glDeleteVertexArrays(1, &plane_vertex_array_object);
    glDeleteBuffers(1, &cube_vertex_buffer_object);
    glDeleteBuffers(1, &plane_vertex_buffer_object);

    glfwTerminate();

    return 0;
}
