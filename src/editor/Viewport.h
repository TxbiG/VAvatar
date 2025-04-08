#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "app.h"

class Viewport
{
public:
    bool init();
    void GUI();

    void Terminate() { app.Terminate(); }

private:
    App app;

    int image_width = 0, image_height = 0;
    const char* current_item = nullptr;
};


#endif // VIEWPORT_H