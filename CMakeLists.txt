cmake_minimum_required(VERSION 3.20)

project(VAvatar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(VAVATAR_ENABLE_OPENCV "Enable OpenCV camera tracking when OpenCV is available" ON)

set(MOSS_FRAMEWORK_DIR "C:/Users/TobyG/Documents/Codex/MossFramework" CACHE PATH "Path to Moss Framework")

if(NOT EXISTS "${MOSS_FRAMEWORK_DIR}/CMakeLists.txt")
    message(FATAL_ERROR "Moss Framework was not found at MOSS_FRAMEWORK_DIR='${MOSS_FRAMEWORK_DIR}'.")
endif()

set(VAVATAR_EXTERNAL_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/external/include")

add_library(imgui STATIC
    ${VAVATAR_EXTERNAL_INCLUDE}/imgui.cpp
    ${VAVATAR_EXTERNAL_INCLUDE}/imgui_draw.cpp
    ${VAVATAR_EXTERNAL_INCLUDE}/imgui_widgets.cpp
    ${VAVATAR_EXTERNAL_INCLUDE}/imgui_tables.cpp
    ${VAVATAR_EXTERNAL_INCLUDE}/imgui_impl_moss.cpp
    ${VAVATAR_EXTERNAL_INCLUDE}/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    ${VAVATAR_EXTERNAL_INCLUDE}
    ${MOSS_FRAMEWORK_DIR}/include
)

# Moss owns platform, renderer, mesh/model and surface implementations.
add_subdirectory("${MOSS_FRAMEWORK_DIR}" "${CMAKE_BINARY_DIR}/_deps/moss" EXCLUDE_FROM_ALL)

add_executable(${PROJECT_NAME} WIN32
    src/main.cpp
    src/editor/app.cpp
    src/editor/VHub.cpp
    src/editor/VEditor.cpp
    src/renderer/MossTexture.cpp
    src/runtime/VStage.cpp
    src/runtime/AvatarRuntime.cpp
    src/core/avatar/AvatarTrackingMapper.cpp
    src/core/avatar/AvatarRigs.cpp
    src/core/project/ProjectDocument.cpp
    src/core/tracking/Audio/AudioTracking.cpp
    src/core/tracking/Camera/TrackingCamera.cpp
    src/core/tracking/Camera/OpenCV/CameraTrackingOpenCV.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${VAVATAR_EXTERNAL_INCLUDE}
    ${MOSS_FRAMEWORK_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    imgui
    Moss::Moss
    opengl32
    gdiplus
    comdlg32
    shell32
    ole32
)

if(VAVATAR_ENABLE_OPENCV)
    find_package(OpenCV QUIET COMPONENTS core imgproc objdetect videoio calib3d face)
    if(OpenCV_FOUND)
        target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})
        target_compile_definitions(${PROJECT_NAME} PRIVATE VAVATAR_ENABLE_OPENCV=1)
    else()
        message(WARNING "OpenCV not found; camera tracking will use preview/simulation mode.")
    endif()
endif()

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3)
    target_link_options(${PROJECT_NAME} PRIVATE /ENTRY:mainCRTStartup)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
