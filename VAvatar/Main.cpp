#include <iostream>
#include <string>
//#include "Windows.h"
//#include <dshow.h>
//#include <Objbase.h>
//#pragma comment(lib, "strmiids")

//#include "editor/import/FBXLoader.h"

#include "editor/VHub.h"

//VHub h;
//#include "editor/VEditor.h"
#include "editor/Viewport.h"

Viewport h;
// To do - 
// Find away to create a opengl view port using ImGui
// Add 3D model into the viewport

int main()
{
    while (h.init()) {
        return 1;
    }

    //VHub hub;
    //ShowWindow(GetConsoleWindow(), SW_HIDE);
    //CoInitializeEx(NULL, COINIT_MULTITHREADED);
    //hub.DisplayDeviceInformation();
    //CoUninitialize();
    //while (hub.init()) {
        //return 1;
    //}
    /*
    if (!hub.running)
    {
        VEditor h;
        while (h.init()) {
            return 1;
        }
    }
    */

    return 0;
}