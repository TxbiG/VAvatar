#include "VEditor.h"
#include <iostream>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include <string>

using namespace std;

bool VEditor::init()
{
    if (!glfwInit())
    {
        // Initialization failed
        cout << "Initialization failed." << endl;
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(640, 480, "VAvatar", NULL, NULL);

    if (!window)
    {
        // Window creation failed
        cout << "Window wasn't created." << endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    //IMGUI_CHECKVERSION();
    //ImGuiContext* ctx = ImGui::CreateContext();
    //ImGui::SetCurrentContext(ctx);
    //ImGuiIO& io = ImGui::GetIO(); (void)io;

    //ImGui::StyleColorsDark();
    //ImGui_ImplGlfw_InitForOpenGL(window, true);
    //ImGui_ImplOpenGL3_Init();

    //const double fpsLimit = 1.0 / 60.0;
    //double lastUpdateTime = 0;
    //double lastFrameTime = 0;

    glfwSwapBuffers(window);

    // Enables the Depth Buffer
    //glEnable(GL_DEPTH_TEST);

    // Main while loop
    while (!glfwWindowShouldClose(window))
    {

        // Swap the back buffer with the front buffer
        glfwSwapBuffers(window);
        // Take care of all GLFW events
        glfwPollEvents();

        render();
        /*
        // Frames per-second
        double now = glfwGetTime();
        double deltaTime = now - lastUpdateTime;
        glfwPollEvents();
        if ((now - lastFrameTime) >= fpsLimit)
        {

            //ImGui_ImplOpenGL3_NewFrame();
            //ImGui_ImplGlfw_NewFrame();
            // draw your frame here
        render();

            //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            lastFrameTime = now;
        }
        lastUpdateTime = now;
        */
    }
    // Free ImGui
    //ImGui_ImplOpenGL3_Shutdown();
    //ImGui_ImplGlfw_Shutdown();
    //ImGui::DestroyContext();
    // Free GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return false;
}

void VEditor::loop()
{
    // FPS 
    input();
    update();
    //render();
}

// Python input value anchors
void VEditor::input()
{
    // Check if input values are the same as before
        // if not dont set the new values

    //av->input();
}

// Render the avatar new position
void VEditor::render()
{
/*
    //av->render();
    ImGui::NewFrame();
    ImGui::Begin("Settings");
    if (ImGui::Button("button"))
    {
        cout << "HI" << endl;
    }
    ImGui::Text("Environment");
    //ImGui::InputInt("Window Width");
    //ImGui::SameLine();
    //ImGui::InputInt("Window Height");
    //ImGui::DragFloat2("Light Intensity", 0.00f, 16.00f);
    //ImGui::ColorPicker3("Colour Lighting");
    ImGui::Text("Model");
    if (ImGui::InvisibleButton("Show Bones", ImVec2(10, 10))) {}
    ImGui::Text("Emotes");
    //ImGui::InputText("Happy");
    //ImGui::InputText("Angery");
    //ImGui::InputText("Sadness");
    //ImGui::InputText("Disgust");
    //ImGui::InputText("Suprised");
    ImGui::Text("Tracking");
    if (ImGui::InvisibleButton("Track Emotions", ImVec2(10, 10))) {}
    if (ImGui::InvisibleButton("Track Facial", ImVec2(10, 10))) {}
    if (ImGui::InvisibleButton("Track Head", ImVec2(10, 10))) {}
    if (ImGui::InvisibleButton("Track UpperBody", ImVec2(10, 10))) {}
    if (ImGui::InvisibleButton("Track Hands", ImVec2(10, 10))) {}
    if (ImGui::InvisibleButton("Track Entire-body", ImVec2(10, 10))) {}

    ImGui::Text("Display");
    if (ImGui::InvisibleButton("Webcam Feed", ImVec2(10, 10))) {}
    if (ImGui::InvisibleButton("Tracking vision", ImVec2(10, 10))) {}
    ImGui::End();

    // VTube view port
    ImGui::Begin("Viewport");
    ImVec2 wsize = ImGui::GetWindowSize();
    // Because I use the texture from OpenGL, I need to invert the V from the UV.
    ImTextureID tex = nullptr;
    ImGui::Image((ImTextureID)tex, wsize, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
    ImGui::EndFrame();
    ImGui::Render();
    */
}
// Update the avatars new position
void VEditor::update()
{
    //av->update();
}