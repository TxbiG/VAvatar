#ifndef VHUB_H
#define VHUB_H

#include "app.h"

using namespace std;

#define MAX_PATH 256

class VHub
{
public:
    bool init();
    void GUI();

    void Terminate() { app.Terminate(); }

private:
    GLuint my_texture = 0;
    int image_width = 0, image_height = 0;
    const char* current_item = nullptr;

    char path[MAX_PATH];
    char nameSize[MAX_PATH];
    std::vector<std::string> cameraNames;
    std::vector<std::string> avatarList;
    int camID = 0;
    bool running = true;


    App app;
};

#endif // VHUB_H