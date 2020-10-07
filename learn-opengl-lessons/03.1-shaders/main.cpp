#include <stdio.h>
#include <memory.h>
#include <math.h>

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
    float vertices[9] =
    {
        -0.5f, -0.5f,  0.0f,
         0.5f, -0.5f,  0.0f,
         0.0f,  0.5f,  0.0f,
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // create vertex shader
    const char *vertex_shader_source = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(position, 1.0);\n"
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
    "uniform vec4 u_color;\n"
    "void main()\n"
    "{\n"
    "   color = u_color;\n"
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

            glUseProgram(shader_program_id);

            // set uniform
            float time_value = glfwGetTime();
            float green_channel = (sin(time_value) / 2.0f) + 0.5f;
            int vertex_color_uniform_id = glGetUniformLocation(shader_program_id, "u_color");
            
            glUniform4f(vertex_color_uniform_id, 0.0f, green_channel, 0.0f, 1.0f);
            glBindVertexArray(vertex_array_id);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
    {
        glDeleteVertexArrays(1, &vertex_array_id);
        glDeleteBuffers(1, &vertex_buffer_id);
        glDeleteBuffers(1, &index_buffer_id);
        glDeleteProgram(shader_program_id);
        glfwTerminate();
    }
    
    return 0;
}
