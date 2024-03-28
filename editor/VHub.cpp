#include "VHub.h"

using namespace std;

// Get FBX filepath copy in header so if pressed create itll add it if not then remove it
// Get Video ID

bool VHub::init()
{
    if (!glfwInit())
    {
        // Initialization failed
        cout << "Initialization failed." << endl;
        return false;
    }

    GLFWwindow* window = glfwCreateWindow(640, 480, "VAvatar", NULL, NULL);

    if (!window)
    {
        // Window creation failed
        cout << "Failed to create window." << endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    //IMGUI_CHECKVERSION();
    //ImGuiContext* ctx = ImGui::CreateContext();
    //ImGui::SetCurrentContext(ctx);
    //ImGuiIO& io = ImGui::GetIO(); (void)io;

    //ImGui::StyleColorsDark();
    //ImGui_ImplGlfw_InitForOpenGL(window, true);
    //ImGui_ImplOpenGL3_Init();

    const double fpsLimit = 1.0 / 24.0;
    double lastUpdateTime = 0;
    double lastFrameTime = 0;
    while (!glfwWindowShouldClose(window) && running == true)
    {
        glfwPollEvents();

        // Frames per-second 24 Frames;
        double now = glfwGetTime();
        double deltaTime = now - lastUpdateTime;
        glfwPollEvents(); //Inputs
        if ((now - lastFrameTime) >= fpsLimit)
        {
            //ImGui_ImplOpenGL3_NewFrame();
            //ImGui_ImplGlfw_NewFrame();
            // draw your frame here
            //render();
            glfwSwapBuffers(window);

            //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            lastFrameTime = now;
        }
        lastUpdateTime = now;
    }

    // Terminate Window setup
    //ImGui_ImplOpenGL3_Shutdown();
    //ImGui_ImplGlfw_Shutdown();
    //ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return false;
}

// To do - Add a stock image of vtuber model as a button
/*
void VHub::render()
{
    ImGui::NewFrame();
    // Start of ImGui Set up
    ///////////////////////////////////////////////////////////
    //ImGui::Begin("VTube");
    if (ImGui::Button("Add Avatar"))
    {
        ImGui::OpenPopup("Add Avatar");
    }
    ImGui::SameLine();
    if (ImGui::Button("Remove"))
    {
    }
    ImGui::SameLine();
    // Get Cameras
    static const char* current_item = NULL;
    if (ImGui::BeginCombo("##combo", current_item, ImGuiComboFlags_NoArrowButton))
    {
        for (int n = 0; n < cameraNames.size(); n++)
        {
            if (ImGui::Selectable((const char*)cameraNames[n].c_str(), true))
            {
                // Get Int ID
                current_item = cameraNames[n].c_str();
                cout << cameraNames[n] << endl; // Get ID
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Selectable("selectable", true);
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
    {
        running = false;
    }

    // Add Button Popup
    if (ImGui::BeginPopupModal("Add Avatar")) {
        ImGui::Text("File path:");
        ImGui::SameLine();
        static char path[MAX_PATH];
        //string filePath;
        if (ImGui::InputText("##filepath", path, MAX_PATH))
        {

        }
        ImGui::SameLine();
        //string icon = "C:/Users/tobyg/source/repos/Vtube/Main/FolderIcon.png";
        //ImTextureID a;
        //if (ImGui::ImageButton(a, ImVec2(64, 64)))
        //{
        //}
        // Open FileDialog
        if (ImGui::Button("##fileicon"))
        {
            getDialog();
        }

        ImGui::Text("Name:");
        ImGui::SameLine();
        static char nameSize[MAX_PATH];
        if (ImGui::InputText("##projectname", nameSize, MAX_PATH))
        {

        }
        if (ImGui::Button("Create"))
        {
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    ImGui::EndFrame();
    ImGui::Render();
    ///////////////////////////////////////////////////////////
    // End of ImGui Set up
}

bool VHub::getDialog() {
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr)) {
        // Create the File Open Dialog object
        IFileDialog* pFileDialog;
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileDialog));

        if (SUCCEEDED(hr)) {
            // Set options to allow only file selection (not open)
            DWORD dwOptions;
            hr = pFileDialog->GetOptions(&dwOptions);
            if (SUCCEEDED(hr)) {
                hr = pFileDialog->SetOptions(dwOptions | FOS_FILEMUSTEXIST | FOS_PATHMUSTEXIST | FOS_FORCEFILESYSTEM);
            }

            if (SUCCEEDED(hr)) {
                COMDLG_FILTERSPEC rgSpec[] = { { L"FBX files", L"*.fbx" } };
                pFileDialog->SetFileTypes(1, rgSpec);

                // Show the dialog
                hr = pFileDialog->Show(NULL);

                // Process the result
                if (SUCCEEDED(hr)) {
                    // Get the file name
                    IShellItem* pShellItem;
                    hr = pFileDialog->GetResult(&pShellItem);
                    if (SUCCEEDED(hr)) {
                        PWSTR pszFilePath;
                        hr = pShellItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                        if (SUCCEEDED(hr)) {
                            // Use pszFilePath here
                            std::wcout << L"Selected file: " << pszFilePath << std::endl;

                            // Check if the "Avatars" folder exists
                            PWSTR documentsPath;
                            hr = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &documentsPath);
                            if (SUCCEEDED(hr)) {
                                std::wstring avatarsFolder = std::wstring(documentsPath) + L"\\Avatars";
                                if (CreateDirectoryW(avatarsFolder.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS) {
                                    std::wcout << L"Avatars folder exists or was created." << std::endl;
                                }
                                else {
                                    std::wcerr << L"Error creating Avatars folder." << std::endl;
                                }
                                CoTaskMemFree(documentsPath);
                            }
                            else {
                                std::wcerr << L"Error getting Documents folder path." << std::endl;
                            }
                        }
                        CoTaskMemFree(pszFilePath);
                    }
                    pShellItem->Release();
                }
            }
            pFileDialog->Release();
        }
        CoUninitialize();
    }
    return false;
}

// Find Devices
HRESULT EnumerateDevices(REFGUID category, IEnumMoniker** ppEnum) {
    ICreateDevEnum* pDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&pDevEnum));
    if (SUCCEEDED(hr)) {
        hr = pDevEnum->CreateClassEnumerator(category, ppEnum, 0);
        if (hr == S_FALSE) {
            hr = VFW_E_NOT_FOUND;  // The category is empty.
        }
        pDevEnum->Release();
    }
    return hr;
}

void VHub::DisplayDeviceInformation() {
    IEnumMoniker* pEnum = NULL;
    if (SUCCEEDED(EnumerateDevices(CLSID_VideoInputDeviceCategory, &pEnum))) {
        IMoniker* pMoniker = NULL;
        while (pEnum->Next(1, &pMoniker, NULL) == S_OK) {
            IPropertyBag* pPropBag;
            if (SUCCEEDED(pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag)))) {
                VARIANT var;
                VariantInit(&var);
                if (SUCCEEDED(pPropBag->Read(L"FriendlyName", &var, 0))) {
                    std::string cameraName = ConvertBSTRToMBS(var.bstrVal);
                    cout << cameraName << endl;
                    cameraNames.push_back(cameraName);
                    VariantClear(&var);
                }
                pPropBag->Release();
            }
            pMoniker->Release();
        }
        pEnum->Release();
    }
    else
    {
        cameraNames.push_back("Null Camera(s)");
        cout << "No Camera(s) found." << endl;
    }
}

std::string VHub::ConvertWCSToMBS(const wchar_t* pstr, long wslen)
{
    int len = ::WideCharToMultiByte(CP_ACP, 0, pstr, wslen, NULL, 0, NULL, NULL);
    std::string dblstr(len, '\0');
    len = ::WideCharToMultiByte(CP_ACP, 0, pstr, wslen, &dblstr[0], len, NULL, NULL);
    return dblstr;
}

std::string VHub::ConvertBSTRToMBS(BSTR bstr)
{
    int wslen = ::SysStringLen(bstr);
    return ConvertWCSToMBS((wchar_t*)bstr, wslen);
}
*/