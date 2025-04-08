#include "VHub.h"

using namespace std;

// Get FBX filepath copy in header so if pressed create itll add it if not then remove it
// Get Video ID
#include "stb/stb_image.h"
#include <fstream>


static bool LoadTextureToImGui(const char* file_path, GLuint out_texture, int out_width, int out_height) {
    // Load the image
    int image_width, image_height, channels;
    unsigned char* image_data = stbi_load(file_path, &image_width, &image_height, &channels, 0);
    if (!image_data) {
        cerr << "Failed to load image! Reason: " << stbi_failure_reason() << endl;
        return false;
    }

    glGenTextures(1, &out_texture);
    glBindTexture(GL_TEXTURE_2D, out_texture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Determine the format
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, format, image_width, image_height, 0, format, GL_UNSIGNED_BYTE, image_data);

    stbi_image_free(image_data); // Free image memory
    out_width = image_width; // Set output width
    out_height = image_height; // Set output height
    return true; // Success
}

bool VHub::init()
{
    if (!app.init("VAvatar Hub", 600, 450)) return false;

    if (!LoadTextureToImGui("editor/nigel.png", my_texture, image_width, image_height))
    {
        return false;
    }

    double lastUpdateTime = glfwGetTime();
    double lastFrameTime = 0.0;
    const double frameDuration = 1.0 / 10.0; // 60 FPS target

    while (!glfwWindowShouldClose(app.window))
    {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastFrameTime;

        if (deltaTime < frameDuration) continue;

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        GUI();

        glfwPollEvents();
        glfwSwapBuffers(app.window);

        lastFrameTime = currentTime;
    }

    return false;
}

void VHub::GUI() {
    // Start a new ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Set up the main window
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("VHub", nullptr, 
                 ImGuiWindowFlags_NoDecoration | 
                 ImGuiWindowFlags_NoResize | 
                 ImGuiWindowFlags_NoTitleBar | 
                 ImGuiWindowFlags_NoCollapse);

    // Title Section
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.9f, 1.0f), "VAvatar Hub");
    ImGui::Separator();

    // Display an Image Banner
    ImGui::Image((ImTextureID)(intptr_t)my_texture, ImVec2(image_width, image_height), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0));
    ImGui::Spacing();
    
    // Avatar Management Section
    ImGui::Text("Manage Your Avatars:");
    ImGui::Separator();
    if (ImGui::Button("Add Avatar", ImVec2(120, 30))) {
        ImGui::OpenPopup("Add Avatar");
    }
    ImGui::SameLine();
    if (ImGui::Button("Remove Avatar", ImVec2(120, 30))) {
        // Add remove logic here
    }

    ImGui::Spacing();

    // Camera Selection Section
    ImGui::Text("Select Camera:");
    ImGui::SameLine();
    if (ImGui::BeginCombo("##camera_combo", current_item, ImGuiComboFlags_NoArrowButton)) {
        for (int n = 0; n < cameraNames.size(); n++) {
            bool isSelected = (current_item == cameraNames[n].c_str());
            if (ImGui::Selectable(cameraNames[n].c_str(), isSelected)) {
                current_item = cameraNames[n].c_str();
                std::cout << "Selected camera: " << current_item << std::endl;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::Spacing();

    // Avatar List Section
    ImGui::Text("Available Avatars:");
    ImGui::Separator();
    for (const auto& avatar : avatarList) { // Assuming avatarList is a vector of avatar names
        if (ImGui::Selectable(avatar.c_str(), false)) {
            // Handle avatar selection logic
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Double-click to edit this avatar");
        }
        if (ImGui::IsMouseDoubleClicked(0)) {
            // Trigger edit logic for the selected avatar
        }
    }

    // Popup for Adding Avatar
    if (ImGui::BeginPopupModal("Add Avatar")) {
        ImGui::Text("Add a New Avatar");
        ImGui::Separator();

        ImGui::Text("File path:");
        ImGui::SameLine();
        if (ImGui::InputText("##filepath", path, MAX_PATH)) {
            // Update file path logic
        }
        ImGui::SameLine();
        if (ImGui::Button("...")) {
            // Open file dialog
        }

        ImGui::Text("Avatar Name:");
        ImGui::SameLine();
        if (ImGui::InputText("##avatar_name", nameSize, MAX_PATH)) {
            // Update avatar name logic
        }

        ImGui::Spacing();

        if (ImGui::Button("Create", ImVec2(100, 30))) {
            // Add avatar creation logic
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(100, 30))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // Footer Section
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "VHub v1.0 - Your Avatar Management Platform");

    ImGui::End(); // End the main window

    // Render the frame
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
