#pragma once

enum Test_Type
{
    MENU            = 0x00,
    CLEAR_COLOR     = 0x01,
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