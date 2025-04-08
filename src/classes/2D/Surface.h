#ifndef SURFACE_H
#define SURFACE_H

#include <glad/glad.h>
#include "editor/classes/3D/Texture.h"
#include "editor/classes/3D/VAO.h"
#include "editor/classes/3D/EBO.h"
#include "variants.h"


// To do -
// Fix roation

class Surface {
public:
    Surface(const char* path, float width, float height, float posX, float posY, Shader& shader);

    void Bind();
    void Delete();

    void SetPosition(float posX, float posY);
    void SetScale(float height, float width);
    void SetRotation(float posX);
    void SetTextureColor(float r, float g, float b);

    float getX() const { return x; }
    float getY() const { return y; }
    float getH() const { return h; }
    float getW() const { return w; }
private:
    float x, y, h, w;
    float rotationAngle;
    Vec2 scale;

    Texture m_tex;
    VAO m_VAO;
    Shader& shader;
    mat4 transform = mat4(1.0f);  // Start with an identity matrix
};
#endif // SURFACE_H