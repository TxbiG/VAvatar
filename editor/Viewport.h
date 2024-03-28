#ifndef VIEWPORT_H
#define VIEWPORT_H

#define STB_IMAGE_IMPLEMENTATION
#define GLM_ENABLE_EXPERIMENTAL

#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include "../rendering/3D/Model.h"

class Viewport
{
public:
    bool init();

    void loop();
    void input();
    void update();
    void render();

    //int running;
};
#endif // VIEWPORT_H