/*
 * Exercise 1
 * - Using the last transformation on the container, try switching the order around
 *   by first rotating and then translating. See what happens and try to reason why this happens
 */

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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        WireFrameEnabled = ~WireFrameEnabled;
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
    float vertices[32] =
    {
        // positions           // texture coords
         0.5f,  0.5f,  0.0f,   1.0f, 1.0f,         // top right
         0.5f, -0.5f,  0.0f,   1.0f, 0.0f,         // bottom right
        -0.5f, -0.5f,  0.0f,   0.0f, 0.0f,         // bottom left
        -0.5f,  0.5f,  0.0f,   0.0f, 1.0f,         // top left
    };
    unsigned vertex_buffer_id;
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // copy index array in an element buffer (index buffer)
    unsigned int indices[6] =
    {
        0, 1, 3, 
        1, 2, 3,
    };
    unsigned int index_buffer_id;
    glGenBuffers(1, &index_buffer_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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


    while (!glfwWindowShouldClose(window))
    {
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
        SetFloatMat4Uniform(shader_program, "u_transform", glm::value_ptr(transform));

        process_input(window);
        {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

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
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
    {
        ReleaseShaderProgram(shader_program);
        glDeleteVertexArrays(1, &vertex_array_id);
        glDeleteBuffers(1, &vertex_buffer_id);
        glDeleteBuffers(1, &index_buffer_id);
        glfwTerminate();
    }
    
    return 0;
}