#include "test.h"
#include "renderer.h"
#include "imgui/imgui.h"

#include <GL/glew.h>

void
OnUpdate_Test_Menu(Test_Main_Menu *m)
{

}

void 
OnRender_Test_Menu(Test_Main_Menu *m)
{

}

void 
OnImGuiRender_Test_Menu(Test_Main_Menu *s)
{
    unsigned int i = s->CurrentTestIndex;
    unsigned int type = s->TestType[i];
    unsigned long int *test = s->Tests[i];
    switch (type)
    {
        case CLEAR_COLOR:
        {
            Test_Clear_Color *t = (Test_Clear_Color *)test;
            OnUpdate_Clear_Color(t, 0.0f);
            OnRender_Clear_Color(t);
            ImGui::Begin("Test");
            if (ImGui::Button("Back"))
            {
                s->CurrentTestIndex = MENU;
            }
            OnImGuiRender_Clear_Color(t);
            ImGui::End();
        } break;

        case TEXTURE_2D:
        {
            if (s->Tests[i] == NULL)
            {
                s->Tests[i] = (unsigned long int *)Create_Test_Texture_2D();   
                test = s->Tests[i];
            }
            Test_Texture_2D *t = (Test_Texture_2D *)test;
            OnUpdate_Texture_2D(t);
            OnRender_Texture_2D(t);
            ImGui::Begin("Test");
            if (ImGui::Button("Back"))
            {
                s->CurrentTestIndex = MENU;
                DeleteShader(t->Shader);
                DeleteVertexBuffer(t->VertexBuffer);
                DeleteIndexBuffer(t->IndexBuffer);
                DeleteVertexArray(t->VertexArray);
                DeleteTexture(t->Texture);
                free(t->VertexBuffer);
                free(t->IndexBuffer);
                free(t->VertexArray);
                free(t->VertexBufferLayout->Elements);
                free(t->VertexBufferLayout);
                free(t->Texture);
                free(s->Tests[i]);
                s->Tests[i] = NULL;
            }
            OnImGuiRender_Texture_2D(t);
            ImGui::End();
        } break;

        default:
        {
            if (ImGui::Button("Clear Color Test"))
            {
                s->CurrentTestIndex = CLEAR_COLOR;
            }
            if (ImGui::Button("Texture 2D Test"))
            {
                s->CurrentTestIndex = TEXTURE_2D;
            }
        };
    }
}

void
OnUpdate_Clear_Color(Test_Clear_Color *t, float deltaTime)
{

}

Test_Texture_2D *
Create_Test_Texture_2D()
{
    Test_Texture_2D *Result = NULL;
    Result = (Test_Texture_2D *)malloc(sizeof(Test_Texture_2D));
    if (Result == NULL)
    {
        return NULL;
    }
    Result->TranslationA = glm::vec3(200.0f, 200.0f, 0.0f);
    Result->TranslationB = glm::vec3(400.0f, 200.0f, 0.0f);

    float vertexBufferData[16] = 
    {
         -50.0f, -50.0f, 0.0f, 0.0f,  // 0
          50.0f, -50.0f, 1.0f, 0.0f,  // 1
          50.0f,  50.0f, 1.0f, 1.0f,  // 2
         -50.0f,  50.0f, 0.0f, 1.0f,  // 3
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
    Result->VertexArray = vertexArray;

    VertexBuffer *vertexBuffer = NewVertexBuffer(vertexBufferData, 16 * sizeof(float));
    ASSERT(vertexBuffer != NULL);
    Result->VertexBuffer = vertexBuffer;

    VertexBufferLayout *vertexBufferLayout = NewVertexBufferLayout(2);
    ASSERT(vertexBufferLayout != NULL);
    Result->VertexBufferLayout = vertexBufferLayout;
    Push(vertexBufferLayout, GL_FLOAT, 2);
    Push(vertexBufferLayout, GL_FLOAT, 2);
    AddBuffer(vertexArray, vertexBuffer, vertexBufferLayout);

    IndexBuffer *indexBuffer = NewIndexBuffer(indexBufferData, 6);
    Result->IndexBuffer = indexBuffer;
    ASSERT(indexBuffer != NULL);

    // projection matrix orthogonal
    Result->ProjectionMatrix = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
    // view matrix (identity matrix) with translation
    Result->ViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    const char *vertexShaderPath = "shaders/vertex.glsl";
    const char *fragmentShaderPath = "shaders/fragment.glsl";
    Shader *shader = NewShader(vertexShaderPath, strlen(vertexShaderPath), fragmentShaderPath, strlen(fragmentShaderPath));
    ASSERT(shader != NULL);
    Result->Shader = shader;
    Bind(shader);

    Texture *texture = NewTexture("..\\assets\\image.png");
    ASSERT(texture != NULL);
    Result->Texture = texture;
    Bind(texture, 0);
    SetUniform1i(shader, "u_texture", 0);

    Unbind(vertexArray);
    Unbind(vertexBuffer);
    Unbind(indexBuffer);
    Unbind(shader);

    return Result;
}

void
OnRender_Clear_Color(Test_Clear_Color *t)
{
    GLCall(glClearColor(t->r, t->b, t->g, t->a));
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void
OnImGuiRender_Clear_Color(Test_Clear_Color *t)
{
    ImGui::ColorEdit4("Clear Color", t->Color);
}

void
OnUpdate_Texture_2D(Test_Texture_2D *t)
{

}

void
OnRender_Texture_2D(Test_Texture_2D *t)
{
    GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT));

    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), t->TranslationA);
    glm::mat4 modelViewProjectionMatrix = t->ProjectionMatrix * t->ViewMatrix * modelMatrix;
    Bind(t->Shader);
    SetUniformMatrix4f(t->Shader, "u_model_view_projection_matrix", modelViewProjectionMatrix);
    Renderer renderer = {};
    Draw(&renderer, t->VertexArray, t->IndexBuffer, t->Shader);

    modelMatrix = glm::translate(glm::mat4(1.0f), t->TranslationB);
    modelViewProjectionMatrix = t->ProjectionMatrix * t->ViewMatrix * modelMatrix;
    Bind(t->Shader);
    SetUniformMatrix4f(t->Shader, "u_model_view_projection_matrix", modelViewProjectionMatrix);
    Draw(&renderer, t->VertexArray, t->IndexBuffer, t->Shader);

}

void
OnImGuiRender_Texture_2D(Test_Texture_2D *t)
{
    ImGui::SliderFloat3("Translation A", &t->TranslationA.x, 0.0f, 960.0f);
    ImGui::SliderFloat3("Translation B", &t->TranslationB.x, 0.0f, 960.0f);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}