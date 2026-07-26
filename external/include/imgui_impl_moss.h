#pragma once

struct Moss_Window;

bool ImGui_ImplMoss_InitForOpenGL(Moss_Window* window);
void ImGui_ImplMoss_Shutdown();
void ImGui_ImplMoss_NewFrame();
