#pragma once

#ifndef VHUB_H
#define VHUB_H

#define GLFW_INCLUDE_NONE

#include <glad/glad.h>
#include <iostream>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "string"
#include "wchar.h"
#include <dshow.h>
#include <vector>

// get file explorer
//#include <ShlObj.h>
//#include <string>
//#include <string.h>

//#pragma comment(lib, "strmiids")

using namespace std;

class VHub
{
public:
	bool init() const;
	//void render();

	//bool getDialog();

	//void get_VideoCap(int i) const { camID = i; }
	int setVideoCap(int device) { return camID = device; }
	//void DisplayDeviceInformation();

	//static std::string ConvertWCSToMBS(const wchar_t* pstr, long wslen);
	//static std::string ConvertBSTRToMBS(BSTR bstr);
	bool running = true;

	static char path[MAX_PATH];
	static char nameSize[MAX_PATH];
	std::vector<std::string> cameraNames;

	// Device
	int camID = { 0 };
};

#endif // VHUB_H