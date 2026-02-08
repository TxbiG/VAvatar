#ifndef APP_H
#define APP_H

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include "GLFW/glfw3.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include <vector>

class App
{
public:
    bool init(const char* name, int width, int height);
    void Terminate();
    
    Moss_Window* window;
};
#endif // VEDITOR_H
