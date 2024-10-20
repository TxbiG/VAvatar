#include "World.h"

//Model* avatar;
Camera3D* camera;

World::World(const char* filename)
{
    //avatar = new Model(filename);
    camera = new Camera3D(640, 480, glm::vec3(0.0, 0.0, 2.0));

    this->shader;

    // Take care of all the light related things
    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);

    shader.Activate();
    glUniform4f(glGetUniformLocation(shader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(shader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
}

World::~World()
{
    shader.Delete();
}

void World::input(GLFWwindow* window)
{
    camera->input(window);
    //avatar->input();
}

void World::update()
{
    camera->updateMatrix(0.0f, 0.0f, 100.0f);
}

void World::render()
{
    //avatar->render();
}