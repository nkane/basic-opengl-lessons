#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include "renderer.cpp"
#include "vertex_buffer.cpp"
#include "index_buffer.cpp"
#include "vertex_array.cpp"
#include "shader.cpp"

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

    float vertexBufferData[8] = 
    {
        -0.5f, -0.5f, // 0
         0.5f, -0.5f, // 1
         0.5f,  0.5f, // 2
        -0.5f,  0.5f, // 3
    };

    unsigned int indexBufferData[6] = 
    {
        0, 1, 2, // right lower triangle
        2, 3, 0, // left upper triangle
    };

    VertexArray *vertexArray = NewVertexArray();
    ASSERT(vertexArray != NULL);

    VertexBuffer *vertexBuffer = NewVertexBuffer((void *)vertexBufferData, 8 * sizeof(float));
    ASSERT(vertexBuffer != NULL);

    VertexBufferLayout *vertexBufferLayout = NewVertexBufferLayout(2);
    ASSERT(vertexBufferLayout != NULL);
    Push(vertexBufferLayout, GL_FLOAT, 2);
    AddBuffer(vertexArray, vertexBuffer, vertexBufferLayout);

    IndexBuffer *indexBuffer = NewIndexBuffer(indexBufferData, 6);
    ASSERT(indexBuffer != NULL);

    const char *vertexShaderPath = "shaders/vertex.glsl";
    const char *fragmentShaderPath = "shaders/fragment.glsl";

    Shader *shader = NewShader(vertexShaderPath, strlen(vertexShaderPath), fragmentShaderPath, strlen(fragmentShaderPath));
    ASSERT(shader != NULL);
    Bind(shader);

    float colors[4] = 
    {
        0.0f, // red
        0.3f, // green
        0.8f, // blue
        1.0f, // alpha
    };

    SetUniform4f(shader, "u_color", colors);

    Unbind(vertexArray);
    Unbind(vertexBuffer);
    Unbind(indexBuffer);
    Unbind(shader);

    float increment = 0.05f;
    while (!glfwWindowShouldClose(window))
    {
        GLCall(glClear(GL_COLOR_BUFFER_BIT));
        {
            Bind(shader);
            SetUniform4f(shader, "u_color", colors);

            Bind(vertexArray);
            Bind(indexBuffer);

            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL));

            if (colors[0] > 1.0f)
            {
                increment = -0.05f;
            }
            else if (colors[0] < 0.0f)
            {
                increment = 0.05f;
            }

            colors[0] += increment;
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    {
        DeleteShader(shader);
        DeleteVertexBuffer(vertexBuffer);
        DeleteIndexBuffer(indexBuffer);
        DeleteVertexArray(vertexArray);
        glfwTerminate();
        free(vertexBuffer);
        free(indexBuffer);
        free(vertexArray);
        free(vertexBufferLayout->Elements);
        free(vertexBufferLayout);
    }

    return 0;
}