#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

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
    unsigned int id = glCreateShader(type); 
    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char *message = (char *)_alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        printf("OpenGL Error Compiling Shader: %s\n", message);
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int
CreateShader(const char *vertexShader, const char *fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompilerShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompilerShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main()
{
    GLFWwindow *window;
    if (!glfwInit())
    {
        return -1;
    }

    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
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

    unsigned int vertexBufferID;
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), &vertexBuffer, GL_STATIC_READ); 
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    unsigned int indexBufferID;
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), &indexBuffer, GL_STATIC_READ);


    const char *vertexShader = ParseShader("shaders/vertex.glsl");
    if (!vertexShader)
    {
        system("pause");
        return -1;
    }
    const char *fragmentShader = ParseShader("shaders/fragment.glsl");
    if (!fragmentShader)
    {
        system("pause");
        return -1;
    }

    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        {
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shader);
    glfwTerminate();

    return 0;
}