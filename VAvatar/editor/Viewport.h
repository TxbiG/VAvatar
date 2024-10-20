#pragma once 

#ifndef VIEWPORT_H
#define VIEWPORT_H

#define STB_IMAGE_IMPLEMENTATION
#define GLM_ENABLE_EXPERIMENTAL
#define GLFW_INCLUDE_NONE

#include<glad/glad.h>
#include<GLFW/glfw3.h>

class Viewport
{
public:
    bool init();

    void loop() const;
    void input() const;
    void update();
    void render();

    //int running;
    GLFWwindow* window;
};
#endif // VIEWPORT_H