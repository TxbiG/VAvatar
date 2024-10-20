#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include "imgui.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <string>
#include <iostream>


#ifndef VEDITOR_H
#define VEDITOR_H

class VEditor
{
public:
    bool init();

    void loop();
    void input();
    void render();
    void update();
};
#endif // VEDITOR_H