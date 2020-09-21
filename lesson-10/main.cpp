#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void
GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool 
GLLogCall(const char *function, const char *file, int line)
{
    while (GLenum error = glGetError())
    {
        printf("[OpenGL Error] : (%u : 0x%x) - %s %s:%d\n", error, error, function, file, line);
        return false;
    }
    return true; 
}

static char *
ParseShader(const char *filePath)
{
    FILE *fileHandle = fopen(filePath, "r");
    if (!fileHandle)
    {
        printf("Failed to load file: %s\n", filePath);
        return NULL;
    }

    char *shaderBuffer = (char *)malloc(1024 * sizeof(char));
    memset(shaderBuffer, 0, 1024 * sizeof(char));
    char lineBuffer[256] = {};

    while(fgets(lineBuffer, 256, fileHandle))
    {
        strcat(shaderBuffer, lineBuffer);
    }

    fclose(fileHandle);
    return shaderBuffer;
}

static unsigned int
CompilerShader(unsigned int type, const char *source)
{
    GLCall(unsigned int id = glCreateShader(type)); 
    GLCall(glShaderSource(id, 1, &source, NULL));
    GLCall(glCompileShader(id));
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

    if (result == GL_FALSE)
    {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char *message = (char *)_alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        printf("OpenGL Error Compiling Shader: %s\n", message);
        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

static unsigned int
CreateShader(const char *vertexShader, const char *fragmentShader)
{
    GLCall(unsigned int program = glCreateProgram());
    GLCall(unsigned int vs = CompilerShader(GL_VERTEX_SHADER, vertexShader));
    GLCall(unsigned int fs = CompilerShader(GL_FRAGMENT_SHADER, fragmentShader));

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));
    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

int main()
{
    GLFWwindow *window;
    if (!glfwInit())
    {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
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

    float vertexBuffer[8] = 
    {
        -0.5f, -0.5f, // 0
         0.5f, -0.5f, // 1
         0.5f,  0.5f, // 2
        -0.5f,  0.5f, // 3
    };

    unsigned int indexBuffer[6] = 
    {
        0, 1, 2, // right lower triangle
        2, 3, 0, // left upper triangle
    };

    unsigned int vertexArrayObjectID;
    GLCall(glGenVertexArrays(1, &vertexArrayObjectID));
    GLCall(glBindVertexArray(vertexArrayObjectID));

    unsigned int vertexBufferID;
    GLCall(glGenBuffers(1, &vertexBufferID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID));
    GLCall(glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), &vertexBuffer, GL_STATIC_READ)); 

    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

    unsigned int indexBufferID;
    GLCall(glGenBuffers(1, &indexBufferID));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), &indexBuffer, GL_STATIC_READ));

    const char *vertexShader = ParseShader("shaders/vertex.glsl");
    ASSERT(vertexShader != NULL);
    const char *fragmentShader = ParseShader("shaders/fragment.glsl");
    ASSERT(fragmentShader != NULL);

    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    GLCall(glUseProgram(shader));

    GLCall(int uniformLocation = glGetUniformLocation(shader, "u_color"));
    ASSERT(uniformLocation != -1);

    GLCall(glBindVertexArray(0));
    GLCall(glUseProgram(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    float colors[4] = 
    {
        0.0f, // red
        0.3f, // green
        0.8f, // blue
        1.0f, // alpha
    };

    float increment = 0.05f;
    while (!glfwWindowShouldClose(window))
    {
        GLCall(glClear(GL_COLOR_BUFFER_BIT));
        {
            GLCall(glUseProgram(shader));
            GLCall(glUniform4f(uniformLocation, colors[0], colors[1], colors[2], colors[3]));

            GLCall(glBindVertexArray(vertexArrayObjectID));
            GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID));

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

    glDeleteProgram(shader);
    glfwTerminate();

    {
        free((void *)vertexShader);
        free((void *)fragmentShader);
    }

    return 0;
}