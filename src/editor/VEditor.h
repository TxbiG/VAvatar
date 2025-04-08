#ifndef VEDITOR_H
#define VEDITOR_H

#include "app.h"

class VEditor
{
public:
    bool init();
    void run();

    void Terminate() { app.Terminate(); }

private:
    App app;

    void showEditor();
    void showPNGTuberProperties();
    void showMainMenu();
    void showCommandConsole();

    ImTextureID te;
    const char* filename = "VAvatar.png";

    static GLuint viewport_texture;
};
#endif // VEDITOR_H