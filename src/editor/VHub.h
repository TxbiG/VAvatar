#ifndef VHUB_H
#define VHUB_H

#include "app.h"

#include <string>
#include <vector>

class VHub
{
public:
    // Run the Hub window until the user chooses Editor, Stage, or Exit.
    HubResult init();

private:
    // Draw the Hub controls for selecting or creating a VTuber project.
    void GUI();

    static constexpr int MaxPathLength = 256;

    const char* current_item = nullptr;
    char path[MaxPathLength] = {};
    char name[MaxPathLength] = {};
    std::vector<std::string> cameraNames = { "Default Moss Camera" };
    std::vector<std::string> avatarList = { "Sample PNGTuber" };
    HubResult result = HubResult::Exit;
    App app;
};

#endif
