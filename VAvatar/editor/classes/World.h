#ifndef WORLD_CLASS_H
#define WORLD_CLASS_H

#include "2D/Camera2D.h"
#include "3D/Model.h"
#include "3D/shaderClass.h"

class World
{
public:
    World(const char* filename);
    ~World();

    void input(GLFWwindow* window);
    void update();
    void render();

    Shader shader = Shader("default.vert", "default.frag");         // Shader and lights
};
#endif // WORLD_CLASS_H