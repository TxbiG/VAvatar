#include "VEditor.h"

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

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    glfwSwapBuffers(window);

    // Enables the Depth Buffer
    glEnable(GL_DEPTH_TEST);

    // Main while loop
    while (!glfwWindowShouldClose(window))
    {

        // Take care of all GLFW events
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        // draw your frame here
        ImGui::NewFrame();
        ImGui::Begin("Editor", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Settings"))
            {
                ImGui::MenuItem("Settings");
                ImGui::MenuItem("ViewPort");
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        if (ImGui::Button("button", ImVec2(10, 10)))
        {
            std::cout << "HI" << std::endl;
        }
        ImGui::Text("Environment");
        //ImGui::InputInt("Window Width");
        //ImGui::SameLine();
        //ImGui::InputInt("Window Height");
        //ImGui::DragFloat2("Light Intensity", 0.50f, 0.01f, 0.01f, 16.00f);
        //ImGui::ColorPicker3("Colour Lighting");
        ImGui::Text("Model");
        if (ImGui::InvisibleButton("Show Bones", ImVec2(10, 10))) {}
        ImGui::Text("Emotes");
        //ImGui::InputText("Happy");
        //ImGui::InputText("Angery");
        //ImGui::InputText("Sadness");
        //ImGui::InputText("Disgust");
        //ImGui::InputText("Suprised");
        //ImGui::Text("Tracking");
        //if (ImGui::InvisibleButton("Track Emotions", ImVec2(10, 10))) {}
        //if (ImGui::InvisibleButton("Track Facial", ImVec2(10, 10))) {}
        //if (ImGui::InvisibleButton("Track Head", ImVec2(10, 10))) {}
        //if (ImGui::InvisibleButton("Track UpperBody", ImVec2(10, 10))) {}
        //if (ImGui::InvisibleButton("Track Hands", ImVec2(10, 10))) {}
        //if (ImGui::InvisibleButton("Track Entire-body", ImVec2(10, 10))) {}

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

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap the back buffer with the front buffer
        glfwSwapBuffers(window);
    }
    // Free ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
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
    //av->render();
    
}
// Update the avatars new position
void VEditor::update()
{
    //av->update();
}