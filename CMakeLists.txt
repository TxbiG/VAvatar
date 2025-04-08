cmake_minimum_required(VERSION 3.16)

project(VAvatar)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Paths to dependencies (adjust these paths as necessary)
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external/include")
set(LIB_DIR "${CMAKE_SOURCE_DIR}/external/lib")

# Source files for your project
set(SOURCES
    main.cpp
    src/editor/VHub.cpp          # Add VHub source file here
    src/editor/VEditor.cpp       # Add VEditor source file here
)

# Include directories
include_directories(${INCLUDE_DIR} src)   # Include your source directory as well

# Link directories (for precompiled libraries like glfw3.lib)
link_directories(${LIB_DIR})

# Add ImGui library
add_library(imgui STATIC
    ${INCLUDE_DIR}/imgui.cpp
    ${INCLUDE_DIR}/imgui_draw.cpp
    ${INCLUDE_DIR}/imgui_impl_glfw.cpp
    ${INCLUDE_DIR}/imgui_impl_opengl3.cpp
    ${INCLUDE_DIR}/imgui_widgets.cpp
    ${INCLUDE_DIR}/imgui_tables.cpp
    ${INCLUDE_DIR}/imgui_demo.cpp
)

# Add GLAD library
add_library(glad STATIC
    src/glad.c
)

# Define the executable target (with WIN32 to avoid console window)
add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

# Link libraries to the executable
target_link_libraries(${PROJECT_NAME}
    glfw3                # Precompiled GLFW library
    opengl32             # OpenGL library (Windows)
    imgui                # Link ImGui library
    glad                 # Link GLAD library
)

# Define preprocessor definitions for ImGui
target_compile_definitions(imgui PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)

# Set the output directories for the executable and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib/Debug")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib/Release")

# Set Debug and Release specific compile flags
set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /Zi /Ob0 /Od /RTC1")
set(CMAKE_C_FLAGS_DEBUG "/D_DEBUG /Zi /Ob0 /Od /RTC1")

set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE "/O2 /DNDEBUG")

# Set the output directory for the executable based on build type
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Add the explicit entry point for the linker
set(CMAKE_EXE_LINKER_FLAGS "/ENTRY:mainCRTStartup")
