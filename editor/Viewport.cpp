#include "Viewport.h"

#define STB_IMAGE_IMPLEMENTATION
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Get File
//Model* model = new Model("VentCurve.fbx");

Model* model = new Model("VentCurve.fbx");

bool Viewport::init()
{
    if (!glfwInit())
    {
        // Initialization failed
        std::cout << "Initialization failed." << std::endl;
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Get GLFW window
    GLFWwindow* window = glfwCreateWindow(640, 480, "VAvatar", NULL, NULL);

    if (!window)
    {
        // Window creation failed
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    gladLoadGL();
    glViewport(0, 0, 640, 480);

    // Generates Shader object using shaders default.vert and default.frag
    //Shader shaderProgram("default.vert", "default.frag");

    // Take care of all the light related things
    //glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    //glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
    //glm::mat4 lightModel = glm::mat4(1.0f);
    //lightModel = glm::translate(lightModel, lightPos);

    //shaderProgram.Activate();
    //glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    //glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);


   // Enables the Depth Buffer
    glEnable(GL_DEPTH_TEST);
    //model->Draw();
    while (!glfwWindowShouldClose(window))
    {
        // Specify the color of the background
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        // Clean the back buffer and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //loop();
        model->Draw();

        // Swap the back buffer with the front buffer
        glfwSwapBuffers(window);
        // Take care of all GLFW events
        glfwPollEvents();
    }

    // Delete all the objects we've created
    //shaderProgram.Delete();
    // Free GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return false;
}

void Viewport::loop()
{
    // Updates and exports the camera matrix to the Vertex Shader
    //camera.updateMatrix(45.0f, 0.1f, 100.0f);

    // Draw a model
    //model.Draw();
}

// Python input value anchors
void Viewport::input()
{
    // Check if input values are the same as before
    // if not dont set the new values

    //model->input();
}

// Update the avatars new position, Rig, Emotion type
void Viewport::update()
{
    //model->update();
}

// Render the avatar new position
void Viewport::render()
{
    //model->render();
}