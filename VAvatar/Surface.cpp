#include "Surface.h"

Surface::Surface(const char* path, float width, float height, float posX, float posY, Shader& shader) : m_tex(path, GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE)
{
    // Calculate the vertices based on width, height, and position
    GLfloat vertices[] = {
        //    Positions                                  colors      texture coords
        posX - width / 2, posY - height / 2, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f, // Lower-left
        posX + width / 2, posY - height / 2, 0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f, // Lower-right
        posX + width / 2, posY + height / 2, 0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 1.0f, // Upper-right
        posX - width / 2, posY + height / 2, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f  // Upper-left
    };

    GLuint indices[] = {
        0, 2, 1, // Upper triangle
        0, 3, 2  // Lower triangle
    };

    this->m_VAO.Bind();

    // Initialize VBO and EBO here, directly using the constructor with parameters
    VBO m_VBO = VBO(vertices, sizeof(vertices)); // Initialize VBO with calculated vertices
    EBO m_EBO = EBO(indices, sizeof(indices)); // Initialize EBO with indices

    this->m_VAO.LinkAttrib(m_VBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);                       // Position attribute (vec3)
    this->m_VAO.LinkAttrib(m_VBO, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));     // Color attribute (vec3)
    this->m_VAO.LinkAttrib(m_VBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));     // Texture coordinates attribute (vec2)

    this->m_VAO.Unbind();
    m_VBO.Unbind();
    m_EBO.Unbind();

    this->m_tex.texUnit(shader, "tex0", 0);
}

void Surface::Bind() {
    this->m_tex.Bind();
    this->m_VAO.Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Surface::Delete() {
    this->m_VAO.Delete();
    this->m_tex.Delete();
}