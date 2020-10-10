/*
 * Exercise 1
 * - Adjust the vertex shader so that the triangle is upside down
 */

#include <stdio.h>
#include <memory.h>
#include <math.h>

#include "shader.cpp"

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

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
        printf("Error: failed to init glew\n");
    }
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // bind vertex array
    unsigned int vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    // copy vertices array in a vertex buffer
    float vertices[18] =
    {
        // positions          // colors
         0.5f, -0.5f,  0.0f,  1.0f, 0.0f, 0.0f,     // bottom right
        -0.5f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,     // bottom left
         0.0f,  0.5f,  0.0f,  0.0f, 0.0f, 1.0f,     // top
    };
    unsigned vertex_buffer_id;
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // copy index array in an element buffer (index buffer)
    unsigned int indices[3] =
    {
        0, 1, 2, 
    };
    unsigned int index_buffer_id;
    glGenBuffers(1, &index_buffer_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // set vertex arrtribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    ShaderProgram *shader_program = CreateShaderProgram("shaders/vertex.glsl", "shaders/fragment.glsl");   

    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
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

            // set uniform
            float time_value = glfwGetTime();
            float green_channel = (sin(time_value) / 2.0f) + 0.5f;
            float color_uniform[4] = { 0.0f, green_channel, 0.0f, 1.0f };
            SetFloat4Uniform(shader_program, "u_color", color_uniform);
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