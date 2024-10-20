#include "VHub.h"

using namespace std;

// Get FBX filepath copy in header so if pressed create itll add it if not then remove it
// Get Video ID
#include "stb/stb_image.h"
#include <fstream>


static bool LoadTextureToImGui(const char* file_path, GLuint out_texture, int out_width, int out_height) {
    // Load the image
    int image_width, image_height, channels;
    unsigned char* image_data = stbi_load(file_path, &image_width, &image_height, &channels, 0);
    if (!image_data) {
        cerr << "Failed to load image! Reason: " << stbi_failure_reason() << endl;
        return false;
    }

    glGenTextures(1, &out_texture);
    glBindTexture(GL_TEXTURE_2D, out_texture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Determine the format
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, format, image_width, image_height, 0, format, GL_UNSIGNED_BYTE, image_data);

    stbi_image_free(image_data); // Free image memory
    out_width = image_width; // Set output width
    out_height = image_height; // Set output height
    return true; // Success
}

bool VHub::init() const
{
    if (!glfwInit())
    {
        // Initialization failed
        cout << "Initialization failed." << endl;
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(640, 480, "VAvatar", NULL, NULL);

    if (!window)
    {
        // Window creation failed
        cout << "Failed to create window." << endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    IMGUI_CHECKVERSION();
    ImGuiContext* ctx = ImGui::CreateContext();
    ImGui::SetCurrentContext(ctx);
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //ImGui::SetWindowSize({});

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    static const char* current_item = NULL;

    const double targetFPS = 1.0;
    const double frameDuration = 1.0 / targetFPS;

    double lastFrameTime = glfwGetTime();
    double currentTime;
    double deltaTime;


    GLuint my_texture = 0; // Initialize your texture variable
    int image_width = 0, image_height = 0;

    // Load texture here
    if (!LoadTextureToImGui("C:\\Users\\tobyg\\source\\repos\\VAvatar\\VAvatar\\editor\\nigel.png", my_texture, image_width, image_height)) {
        std::cerr << "Failed to load nigel.png." << std::endl;
        // Handle failure case, maybe set a flag or load a default texture
    }


    while (!glfwWindowShouldClose(window) && running == true)
    {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrameTime;
        if (deltaTime < frameDuration)
        {
            // Start of ImGui Set up
            ///////////////////////////////////////////////////////////
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("VHub", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
            ImGui::Image((ImTextureID)(intptr_t)my_texture, ImVec2(image_width, image_height));

            //ImGui::SameLine();
            //ImGui::ImageButton();
            //ImGui::ImageButton()
            //ImGui::ImageButton();
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
            ImGui::TextUnformatted(" Camera:");
            ImGui::SameLine();
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
                //running = false;
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
                    //getDialog();
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

            ImGui::End();
            ImGui::EndFrame();
            ImGui::Render();

            glfwSwapBuffers(window);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
        lastFrameTime = currentTime;

        glfwPollEvents(); // Input
    }

    // Terminate Window setup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return false;
}

/*
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