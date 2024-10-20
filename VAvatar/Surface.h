#ifndef SURFACE_H
#define SURFACE_H

#include <glad/glad.h>
#include "editor/classes/3D/Texture.h"
#include "editor/classes/3D/VAO.h"
#include "editor/classes/3D/EBO.h"

class Surface {
public:
    Surface(const char* path, float width, float height, float posX, float posY, Shader& shader);

    void Bind();
    void Delete();

private:
    Texture m_tex;
    VAO m_VAO;
};
#endif // SURFACE_H