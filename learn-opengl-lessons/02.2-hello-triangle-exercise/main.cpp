/*
 *  Exercise 2 
 *  - create the same 2 triangles using two different VAOs and VBOs for their data 
 */
 

#include <stdio.h>
#include <memory.h>

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
    unsigned int vertex_array_1_id;
    glGenVertexArrays(1, &vertex_array_1_id);
    glBindVertexArray(vertex_array_1_id);
    float vertices_1[9] =
    {
        // first triangle
        -1.00f,  0.00f,  0.00f,
        -0.50f,  0.00f,  0.00f,
        -0.75f,  0.50f,  0.00f,
    };
    // copy vertices array in a vertex buffer
    unsigned vertex_buffer_1_id;
    glGenBuffers(1, &vertex_buffer_1_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_1_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_1), vertices_1, GL_STATIC_DRAW);
    // set vertex arrtribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // bind vertex array
    unsigned int vertex_array_2_id;
    glGenVertexArrays(1, &vertex_array_2_id);
    glBindVertexArray(vertex_array_2_id);
    float vertices_2[9] = 
    {
        // second triangle
         0.25f,  0.00f,  0.00f,
         0.75f,  0.00f,  0.00f,
         0.50f,  0.50f,  0.00f,
    };
    // copy vertices array in a vertex buffer
    unsigned int vertex_buffer_2_id;
    glGenBuffers(1, &vertex_buffer_2_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_2_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_2), vertices_2, GL_STATIC_DRAW);
    // set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // create vertex shader
    const char *vertex_shader_source = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
    "}\0";
    unsigned int vertex_shader_id;
    vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_id, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader_id);
    int success;
    char info_logs[512];
    glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader_id, 512, NULL, info_logs);
        printf("[Error - Vertex Shader]: %s\n", info_logs);
        return -1;
    }
    memset(info_logs, 0, 512);

    // create fragment shader
    const char *fragment_shader_source =
    "#version 330 core\n"
    "out vec4 color;\n"
    "void main()\n"
    "{\n"
    "   color = vec4(1.0, 0.5, 0.2, 1.0f);\n"
    "}\0";
    unsigned int fragment_shader_id;
    fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_id, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader_id);
    glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment_shader_id, 512, NULL, info_logs);
        printf("[Error - Fragment Shader]: %s\n", info_logs);
    }
    memset(info_logs, 0, 512);

    // create shader program linking vertex and fragment shader
    unsigned int shader_program_id;
    shader_program_id = glCreateProgram();
    glAttachShader(shader_program_id, vertex_shader_id);
    glAttachShader(shader_program_id, fragment_shader_id);
    glLinkProgram(shader_program_id);
    glGetProgramiv(shader_program_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader_program_id, 512, NULL, info_logs);
        printf("[Error - Program Linking]: %s\n", info_logs);
    }
    memset(info_logs, 0, 512);
    {
        // shaders no longer needed, since linked to program
        glDeleteShader(vertex_shader_id);
        glDeleteShader(fragment_shader_id);
    }

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
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glUseProgram(shader_program_id);
            glBindVertexArray(vertex_array_1_id);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_1_id);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(vertex_array_2_id);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_2_id);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
    {
        glDeleteVertexArrays(1, &vertex_array_1_id);
        glDeleteBuffers(1, &vertex_buffer_1_id);
        glDeleteVertexArrays(1, &vertex_array_2_id);
        glDeleteBuffers(1, &vertex_buffer_2_id);
        glDeleteProgram(shader_program_id);
        glfwTerminate();
    }
    
    return 0;
}
