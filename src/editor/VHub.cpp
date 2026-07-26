#include "VHub.h"

HubResult VHub::init()
{
    if (!app.init("VAvatar Hub", 600, 450)) {
        return HubResult::Exit;
    }

    current_item = cameraNames.empty() ? nullptr : cameraNames.front().c_str();

    while (app.beginFrame()) {
        GUI();
        app.endFrame();
    }

    return result;
}

void VHub::GUI()
{
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

    ImGui::Begin("VHub", nullptr,
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse);

    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.9f, 1.0f), "VAvatar Hub");
    ImGui::Separator();

    ImGui::Text("Manage Your Avatars");
    if (ImGui::Button("Add Avatar", ImVec2(120, 30))) {
        ImGui::OpenPopup("Add Avatar");
    }

    ImGui::SameLine();
    if (ImGui::Button("Open Editor", ImVec2(120, 30))) {
        result = HubResult::OpenEditor;
        app.close();
    }

    ImGui::SameLine();
    if (ImGui::Button("Go Live", ImVec2(120, 30))) {
        result = HubResult::StartStage;
        app.close();
    }

    ImGui::Spacing();
    ImGui::Text("Select Camera:");
    ImGui::SameLine();

    if (ImGui::BeginCombo("##camera_combo", current_item)) {
        for (const std::string& camera : cameraNames) {
            const bool selected = current_item == camera.c_str();
            if (ImGui::Selectable(camera.c_str(), selected)) {
                current_item = camera.c_str();
            }
            if (selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Spacing();
    ImGui::Text("Available Avatars");
    ImGui::Separator();
    for (const auto& avatar : avatarList) {
        ImGui::Selectable(avatar.c_str(), false);
    }

    if (ImGui::BeginPopupModal("Add Avatar", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("File path", path, MaxPathLength);
        ImGui::InputText("Avatar name", name, MaxPathLength);

        if (ImGui::Button("Create", ImVec2(100, 30))) {
            if (name[0] != '\0') {
                avatarList.emplace_back(name);
                name[0] = '\0';
                path[0] = '\0';
            }
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(100, 30))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Running on Moss platform");

    ImGui::End();
}
