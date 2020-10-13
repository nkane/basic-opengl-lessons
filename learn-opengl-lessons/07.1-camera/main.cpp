#include <stdio.h>
#include <memory.h>
#include <math.h>

#include "shader.cpp"

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb\stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static unsigned int WireFrameEnabled = 0;

glm::vec3 camera_position   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 camera_front      = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up         = glm::vec3(0.0f, 1.0f, 0.0f);

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

    const float camera_speed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera_position += camera_speed * camera_front;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera_position -= camera_speed * camera_front;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera_position -= glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera_position += glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
    }
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

    if (glewInit() != GLEW_OK)
    {
        printf("[GLEW ERROR]: failed to init glew\n");
    }
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    int width;
    int height;
    int channels;

    unsigned char *data = stbi_load("assets/container.jpg", &width, &height, &channels, 0);
    if (!data)
    {
        printf("[STB_Image ERROR]: failed to load image\n");
        return -1;
    }
    // generate and bind texture
    unsigned int container_texture_id;
    glGenTextures(1, &container_texture_id);
    // activate the texture unit first before binding texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, container_texture_id);
    // set texture wrapping and filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    data = NULL;
    width = 0;
    height = 0;
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("assets/awesomeface.png", &width, &height, &channels, 0);
    if (!data)
    {
        printf("[STB_Image ERROR]: failed to load image\n");
        return -1;
    }
    unsigned int face_texture_id;
    glGenTextures(1, &face_texture_id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, face_texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    // bind vertex array
    unsigned int vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    // copy vertices array in a vertex buffer
    float vertices[] =
    {
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
    unsigned vertex_buffer_id;
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // set vertex arrtribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    ShaderProgram *shader_program = CreateShaderProgram("shaders/vertex.glsl", "shaders/fragment.glsl");   
    glUseProgram(shader_program->id);
    SetIntUniform(shader_program, "u_texture_1", 0);
    SetIntUniform(shader_program, "u_texture_2", 1);

    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);

    glm::vec3 cube_position[] =
    {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f) 
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

            glUseProgram(shader_program->id);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, container_texture_id);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, face_texture_id);

            glBindVertexArray(vertex_array_id);

            for (unsigned int i = 0; i < 10; i++)
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, cube_position[i]);
                float angle = 0.0f;
                if (i == 1 || (i % 3) == 0)
                {
                    angle = 20.0f * i;
                }
                model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                const float radius = 10.0f;
                float camera_x = sin(glfwGetTime()) * radius;
                float camera_z = cos(glfwGetTime()) * radius;
                glm::mat4 view = glm::mat4(1.0f);
                view = glm::lookAt(camera_position, camera_position + camera_front, camera_up);
                glm::mat4 perspective_projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
                SetFloatMat4Uniform(shader_program, "u_model", glm::value_ptr(model));
                SetFloatMat4Uniform(shader_program, "u_view", glm::value_ptr(view));
                SetFloatMat4Uniform(shader_program, "u_projection", glm::value_ptr(perspective_projection));
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
    {
        ReleaseShaderProgram(shader_program);
        glDeleteVertexArrays(1, &vertex_array_id);
        glDeleteBuffers(1, &vertex_buffer_id);
        glfwTerminate();
    }
    
    return 0;
}