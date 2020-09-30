#pragma once

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

void
OnUpdate_Clear_Color(Test_Clear_Color *t, float deltaTime);

void
OnRender_Clear_Color(Test_Clear_Color *t);

void
OnImGuiRender_Clear_Color(Test_Clear_Color *t);