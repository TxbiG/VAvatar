#ifndef VAVATAR_APP_H
#define VAVATAR_APP_H

#include <Moss/Moss_Platform.h>

#include <imgui.h>
#include <imgui_impl_moss.h>
#include <imgui_impl_opengl3.h>

#include <string>


enum class HubResult {
    Exit,
    OpenEditor,
    StartStage
};

enum class EditorResult {
    Exit,
    BackToHub,
    StartStage
};

enum class StageResult {
    Exit,
    BackToEditor
};


class App {
public:
    App() = default;
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    // Shut down ImGui and terminate the Moss window.
    ~App();

    // Create a Moss-backed OpenGL window and initialize ImGui for it.
    bool init(const char* title, int width, int height);

    // Poll input, begin a new ImGui frame, and clear the window.
    bool beginFrame(float clear_r = 0.1f, float clear_g = 0.1f, float clear_b = 0.1f, float clear_a = 1.0f);

    // Render the current ImGui frame and present the window.
    void endFrame();

    // Return whether the underlying window has been asked to close.
    bool shouldClose() const;

    // Request a clean close at the end of the current app flow.
    void close();

    Moss_Window* window = nullptr;

private:
    bool imguiReady = false;
};

#endif
