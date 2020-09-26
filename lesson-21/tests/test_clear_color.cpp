#include "test_clear_color.h"
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

        default:
        {
            if (ImGui::Button("Clear Color Test"))
            {
                s->CurrentTestIndex = CLEAR_COLOR;
            }
        };
    }
}

void
OnUpdate_Clear_Color(Test_Clear_Color *t, float deltaTime)
{

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