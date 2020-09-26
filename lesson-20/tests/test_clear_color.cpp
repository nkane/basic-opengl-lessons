#include "test_clear_color.h"
#include "renderer.h"
#include "imgui/imgui.h"

#include <GL/glew.h>

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