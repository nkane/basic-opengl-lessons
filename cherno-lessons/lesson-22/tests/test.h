#pragma once

#include "glm/glm.hpp"
#include "shader.h"
#include "texture.h"

enum Test_Type
{
    MENU            = 0x00,
    CLEAR_COLOR     = 0x01,
    TEXTURE_2D      = 0x02,
    QUAD            = 0X03,
};

struct Test_Clear_Color
{
    union
    {
        float Color[4];
        struct
        {
            float r;
            float b;
            float g;
            float a;
        };
    };
};

struct Test_Texture_2D
{
    glm::vec3 TranslationA;
    glm::vec3 TranslationB;
    glm::mat4 ProjectionMatrix;
    glm::mat4 ViewMatrix;
    Shader *Shader;
    Texture *Texture;
    VertexArray *VertexArray;
    VertexBuffer *VertexBuffer;
    VertexBufferLayout *VertexBufferLayout;
    IndexBuffer *IndexBuffer;
};

struct Test_Quad
{
    float Vertices[12];
    unsigned int Indices[6];
    VertexBuffer *VertexBuffer;
    VertexBufferLayout *VertexBufferLayout;
    VertexArray *VertexArray;
    IndexBuffer *IndexBuffer;
};

struct Test_Main_Menu
{
    unsigned long int *Tests[32];
    Test_Type TestType[32];
    unsigned int CurrentTestIndex;
};

void
OnUpdate_Test_Menu(Test_Main_Menu *m);

void 
OnRender_Test_Menu(Test_Main_Menu *m);

void 
OnImGuiRender_Test_Menu(Test_Main_Menu *m);

void
OnUpdate_Clear_Color(Test_Clear_Color *t, float deltaTime);

void
OnRender_Clear_Color(Test_Clear_Color *t);

void
OnImGuiRender_Clear_Color(Test_Clear_Color *t);

Test_Texture_2D *
Create_Test_Texture_2D();

void
OnUpdate_Texture_2D(Test_Texture_2D *t);

void
OnRender_Texture_2D(Test_Texture_2D *t);

void
OnImGuiRender_Texture_2D(Test_Texture_2D *t);

Test_Quad *
Create_Test_Quad();

void
OnUpdate_Test_Quad(Test_Quad *t);

void
OnRender_Test_Quad(Test_Quad *t);

void
OnImGuiRender_Test_Quad(Test_Quad *t);