#include "VEditor.h"
#include "../classes/2D/PNGTuber.h"   // PNGTuber-specific header
#include "../classes/2D/Live2DModule.h" //Live2D functionality
#include "../classes/2D/Model3DModule.h" // Hypothetical 3D functionality

GLuint createFramebuffer(const ImVec2& size, GLuint& texture) {
    static GLuint framebuffer = 0;

    if (framebuffer == 0) {
        glGenFramebuffers(1, &framebuffer);
        glGenTextures(1, &texture);

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)size.x, (int)size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Resize framebuffer texture if necessary
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)size.x, (int)size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    return framebuffer;
}

void VEditor::GUI() {
    // Enable docking
    static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockspaceFlags);

    showMainMenu();
    showEditorPanels();
}

void VEditor::showMainMenu() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New")) { /* Logic for creating a new project */ }
            if (ImGui::MenuItem("Open")) { /* Logic for opening a project */ }
            if (ImGui::MenuItem("Save")) { /* Logic for saving a project */ }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) { /* Exit logic */ }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo")) { /* Undo logic */ }
            if (ImGui::MenuItem("Redo")) { /* Redo logic */ }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Reset Layout")) { /* Reset docking layout */ }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) { /* Show about dialog */ }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void VEditor::showEditorPanels() {
    // Main viewport
    if (ImGui::Begin("Viewport")) {
        renderViewport();
    }
    ImGui::End();

    // Property editor
    if (ImGui::Begin("Properties")) {
        switch (currentMode) {
            case EditMode::PNG_TUBER:
                showPNGTuberProperties();
                break;
            case EditMode::LIVE2D:
                showLive2DProperties();
                break;
            case EditMode::MODE_3D:
                show3DProperties();
                break;
        }
    }
    ImGui::End();

    // Node editor
    if (ImGui::Begin("Node Editor")) {
        showNodeEditor();
    }
    ImGui::End();

    // Command console
    if (ImGui::Begin("Console")) {
        showCommandConsole();
    }
    ImGui::End();
}

void VEditor::renderViewport() {
    // Clear viewport and render the active scene
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    if (currentMode == EditMode::PNG_TUBER) {
        render2DViewport(viewportSize);
    } else if (currentMode == EditMode::LIVE2D) {
        renderLive2DViewport(viewportSize);
    } else if (currentMode == EditMode::MODE_3D) {
        render3DViewport(viewportSize);
    }
}

void VEditor::render2DViewport(const ImVec2& size) {
    glm::mat4 orthoProj = glm::ortho(0.0f, size.x, 0.0f, size.y, -1.0f, 1.0f);

    // Optional: Add camera movement (pan, zoom)
    glm::vec3 cameraPosition = glm::vec3(cameraX, cameraY, 0.0f); // Camera offset
    glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), -cameraPosition);
    glm::mat4 viewProjection = orthoProj * viewMatrix;

    GLuint framebuffer = createFramebuffer(size, viewport_texture);

    // Bind framebuffer and render
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, (int)size.x, (int)size.y);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader2D.use();
    shader2D.setUniform("viewProjection", viewProjection);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Display framebuffer texture in ImGui
    ImGui::Image((void*)(intptr_t)texture, size, ImVec2(0, 1), ImVec2(1, 0));
}

void VEditor::render3DViewport(const ImVec2& size) {
    float aspectRatio = size.x / size.y;
    glm::mat4 perspectiveProj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

    glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f); // Camera position
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);   // Camera looks at the origin
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);       // "Up" is along Y-axis
    glm::mat4 viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);

    glm::mat4 viewProjection = perspectiveProj * viewMatrix;

    GLuint framebuffer = createFramebuffer(size, viewport_texture);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, (int)size.x, (int)size.y);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind the 3D shader
    shader3D.use();
    shader3D.setUniform("viewProjection", viewProjection);

    // Render 3D models
    render3DModels();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Display framebuffer texture in ImGui
    ImGui::Image((void*)(intptr_t)viewport_texture, size, ImVec2(0, 1), ImVec2(1, 0));
}

bool VEditor::init()
{
    if (!app.init("VAvatar Hub", 600, 450)) return false;

    if (!LoadTextureToImGui("editor/nigel.png", my_texture, image_width, image_height))
    {
        return false;
    }

    double lastUpdateTime = glfwGetTime();
    double lastFrameTime = 0.0;
    const double frameDuration = 1.0 / 60.0; // 60 FPS target

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

void VEditor::showPNGTuberProperties()
{
    ImGui::Text("PNGTuber Properties");
    ImGui::Separator();
    ImGui::SliderFloat("Bounce", &angle, -360, 360);
    ImGui::SliderFloat("Squish", &angle, -360, 360);
    ImGui::SliderFloat("Frequency", &angle, -360, 360);
    ImGui::ColorPicker4("Tint", &tintColor);
}

void VEditor::showLive2DProperties() {
    ImGui::Text("Live2D Properties");
    ImGui::Separator();
    ImGui::Text("Animation Control");
    // Add sliders for Live2D-specific properties
}

void VEditor::show3DProperties() {
    ImGui::Text("3D Model Properties");
    ImGui::Separator();
    ImGui::Text("Transform");
    ImGui::DragFloat3("Position", &position.x, 0.1f);
    ImGui::DragFloat3("Rotation", &rotation.x, 0.1f);
    ImGui::DragFloat3("Scale", &scale.x, 0.1f);
}

void VEditor::showNodeEditor() {
    if (ImGui::TreeNode("Scene")) {
        // Node hierarchy
        for (const auto& node : sceneGraph) {
            if (ImGui::TreeNode(node.name.c_str())) {
                // Node properties
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }
}

void VEditor::showCommandConsole() {
    ImGui::Text("Console Output:");
    // Scrollable console logic
}