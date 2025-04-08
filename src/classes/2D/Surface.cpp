#include "Surface.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Surface::Surface(const char* path, float width, float height, float posX, float posY, Shader& shader) : m_tex(path, GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE), shader(shader), x(posX), y(posY), w(width), h(height), scale(1.0f, 1.0f), rotationAngle(0.0f)
{
    x = posX;
    y = posY;
    h = height;
    w = width;

    // Calculate the vertices based on width, height, and position
    GLfloat vertices[] = {
        //    Positions                                  colors      texture coords
        x - w / 2, y - h / 2, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, // Lower-left
        x + w / 2, y - h / 2, 0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, // Lower-right
        x + w / 2, y + h / 2, 0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f, // Upper-right
        x - w / 2, y + h / 2, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f  // Upper-left
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

    // Call defaults
    SetRotation(1.0);
    SetTextureColor(1.0f, 1.0f, 1.0f);
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

void Surface::SetPosition(float posX, float posY) {
    // Update the position
    x = posX;
    y = posY;

    // Recalculate vertices with new position
    GLfloat vertices[] = {
        //    Positions                                  colors      texture coords
        x - w / 2, y - h / 2, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f, // Lower-left
        x + w / 2, y - h / 2, 0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f, // Lower-right
        x + w / 2, y + h / 2, 0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 1.0f, // Upper-right
        x - w / 2, y + h / 2, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f  // Upper-left
    };

    // Re-bind the VBO and update its data with the new vertices
    m_VAO.Bind();
    VBO m_VBO = VBO(vertices, sizeof(vertices));
    m_VAO.LinkAttrib(m_VBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);                       // Position attribute

    // Unbind the VBO and VAO
    m_VAO.Unbind();
    m_VBO.Unbind();
}

void Surface::SetScale(float scaleX, float scaleY) {
    // Update the position
    w = scaleX;
    h = scaleY;

    // Recalculate vertices with new position
    GLfloat vertices[] = {
        //    Positions                                  colors      texture coords
        x - w / 2, y - h / 2, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f, // Lower-left
        x + w / 2, y - h / 2, 0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f, // Lower-right
        x + w / 2, y + h / 2, 0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 1.0f, // Upper-right
        x - w / 2, y + h / 2, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f  // Upper-left
    };

    // Re-bind the VBO and update its data with the new vertices
    m_VAO.Bind();
    VBO m_VBO = VBO(vertices, sizeof(vertices));
    m_VAO.LinkAttrib(m_VBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);                       // Position attribute
    m_VAO.LinkAttrib(m_VBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));     // Texture coordinates attribute (vec2)

    // Unbind the VBO and VAO
    m_VAO.Unbind();
    m_VBO.Unbind();
}

void Surface::SetRotation(float angle) {
    float scale = 1.0f;         // Set your scaling factor
    float _angle = glm::radians(angle);  // Set your rotation angle (45 degrees)

    // Apply scaling
    transform = glm::scale(transform, glm::vec3(scale, scale, 1.0f));

    // Apply rotation around Z-axis
    transform = glm::rotate(transform, _angle, glm::vec3(0.0f, 0.0f, 1.0f));

    GLuint transformLoc = glGetUniformLocation(shader.ID, "transform");
    if (transformLoc == -1) {
        std::cerr << "Error: Uniform 'transform' not found in shader." << std::endl;
        return;
    }
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
}

void Surface::SetTextureColor(float r, float g, float b) {
    GLuint colorLoc = glGetUniformLocation(shader.ID, "tintColor");
    glUniform3f(colorLoc, r, g, b);  // Set the uniform color in the shader
}