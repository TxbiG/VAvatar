#include "app.h"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <GL/gl.h>

bool App::init(const char* title, int width, int height)
{
    window = Moss_CreateWindow(title, width, height, nullptr, nullptr);
    if (!window) {
        return false;
    }

    Moss_MakeContextCurrent(window);
    Moss_SwapBuffersInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    const bool platformReady = ImGui_ImplMoss_InitForOpenGL(window);
    const bool rendererReady = platformReady && ImGui_ImplOpenGL3_Init("#version 130");
    imguiReady = platformReady && rendererReady;
    if (!imguiReady) {
        if (platformReady) ImGui_ImplMoss_Shutdown();
        ImGui::DestroyContext();
        return false;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, width, height);

    return imguiReady;
}

bool App::beginFrame(float clear_r, float clear_g, float clear_b, float clear_a)
{
    if (!window || shouldClose()) {
        return false;
    }

    Moss_PollEvents();

    glViewport(0, 0, Moss_GetWindowWidth(), Moss_GetWindowHeight());
    glClearColor(clear_r, clear_g, clear_b, clear_a);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplMoss_NewFrame();
    ImGui::NewFrame();
    return true;
}

void App::endFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    Moss_SwapBuffers();
}

bool App::shouldClose() const
{
    return !window || Moss_ShouldWindowClose(window);
}

void App::close()
{
    if (window) {
        Moss_CloseWindow(window);
    }
}

App::~App()
{
    if (imguiReady) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplMoss_Shutdown();
        ImGui::DestroyContext();
    }

    if (window) {
        Moss_TerminateWindow(window);
        window = nullptr;
    }
}
