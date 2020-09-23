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
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "renderer.cpp"
#include "vertex_buffer.cpp"
#include "index_buffer.cpp"
#include "vertex_array.cpp"
#include "shader.cpp"
#include "texture.cpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

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

    window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
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
         100.0f, 100.0f, 0.0f, 0.0f,  // 0
         200.0f, 100.0f, 1.0f, 0.0f,  // 1
         200.0f, 200.0f, 1.0f, 1.0f,  // 2
         100.0f, 200.0f, 0.0f, 1.0f,  // 3
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

    // projection matrix orthogonal
    glm::mat4 projectionMatrix = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
    // view matrix (identity matrix) with translation
    glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 0.0f, 0.0f));
    

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

    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window, true);
    ImGui::StyleColorsDark();

    glm::vec3 translation(200.0f, 200.0f, 0.0f);

    while (!glfwWindowShouldClose(window))
    {
        Clear(&renderer);
        ImGui_ImplGlfwGL3_NewFrame();

        // model matrix
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), translation);
        // model view projection matrix
        glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;

        Bind(shader);
        Draw(&renderer, vertexArray, indexBuffer, shader);
        SetUniformMatrix4f(shader, "u_model_view_projection_matrix", modelViewProjectionMatrix);

        ImGui::SliderFloat3("Translation", &translation.x, 0.0f, 960.0f); 
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    {
        ImGui_ImplGlfwGL3_Shutdown();
        ImGui::DestroyContext();
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
        free(texture);
    }

    return 0;
}