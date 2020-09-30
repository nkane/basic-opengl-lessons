#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "renderer.cpp"
#include "vertex_buffer.cpp"
#include "index_buffer.cpp"
#include "vertex_array.cpp"
#include "shader.cpp"
#include "texture.cpp"

int main()
{
    GLFWwindow *window;
    if (!glfwInit())
    {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if(glewInit() != GLEW_OK)
    {
        printf("Error: failed to init glew\n");
    }
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

    float vertexBufferData[16] = 
    {
        -0.5f, -0.5f, 0.0f, 0.0f,  // 0
         0.5f, -0.5f, 1.0f, 0.0f,  // 1
         0.5f,  0.5f, 1.0f, 1.0f,  // 2
        -0.5f,  0.5f, 0.0f, 1.0f,  // 3
    };

    unsigned int indexBufferData[6] = 
    {
        0, 1, 2, // right lower triangle
        2, 3, 0, // left upper triangle
    };

    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC1_ALPHA));
    GLCall(glEnable(GL_BLEND));

    VertexArray *vertexArray = NewVertexArray();
    ASSERT(vertexArray != NULL);

    VertexBuffer *vertexBuffer = NewVertexBuffer(vertexBufferData, 16 * sizeof(float));
    ASSERT(vertexBuffer != NULL);

    VertexBufferLayout *vertexBufferLayout = NewVertexBufferLayout(2);
    ASSERT(vertexBufferLayout != NULL);
    Push(vertexBufferLayout, GL_FLOAT, 2);
    Push(vertexBufferLayout, GL_FLOAT, 2);
    AddBuffer(vertexArray, vertexBuffer, vertexBufferLayout);

    IndexBuffer *indexBuffer = NewIndexBuffer(indexBufferData, 6);
    ASSERT(indexBuffer != NULL);

    const char *vertexShaderPath = "shaders/vertex.glsl";
    const char *fragmentShaderPath = "shaders/fragment.glsl";

    Shader *shader = NewShader(vertexShaderPath, strlen(vertexShaderPath), fragmentShaderPath, strlen(fragmentShaderPath));
    ASSERT(shader != NULL);
    Bind(shader);

    Texture *texture = NewTexture("..\\assets\\image.png");
    ASSERT(texture != NULL);
    Bind(texture, 0);
    SetUniform1i(shader, "u_texture", 0);

    Unbind(vertexArray);
    Unbind(vertexBuffer);
    Unbind(indexBuffer);
    Unbind(shader);

    Renderer renderer = {};

    float increment = 0.05f;
    while (!glfwWindowShouldClose(window))
    {
        Clear(&renderer);
        Bind(shader);
        Draw(&renderer, vertexArray, indexBuffer, shader);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    {
        DeleteShader(shader);
        DeleteVertexBuffer(vertexBuffer);
        DeleteIndexBuffer(indexBuffer);
        DeleteVertexArray(vertexArray);
        DeleteTexture(texture);
        glfwTerminate();
        free(vertexBuffer);
        free(indexBuffer);
        free(vertexArray);
        free(vertexBufferLayout->Elements);
        free(vertexBufferLayout);
    }

    return 0;
}