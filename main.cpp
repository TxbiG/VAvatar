#include "src/editor/VHub.h"  // VHub class
#include "src/editor/VEditor.h"  // VEditor class
//#include "src/editor/Viewport.h" // Optional Viewport class

enum class AppState { VHub, VEditor, Viewport, Exit };

/*
mkdir build
cd build
cmake ..
cmake --build .
*/
int main() {
    AppState state = AppState::VHub;

    // Main loop
    while (state != AppState::Exit) {
        switch (state) {
            case AppState::VHub: {
                VHub hub; // Pass shared App context
                if (hub.init()) { state = AppState::VEditor; } // Transition to Editor
                else { state = AppState::Exit; }  // Exit app
                break;
            }

            case AppState::VEditor: {
                VEditor editor;
                if (editor.init()) { state = AppState::Viewport; } // Transition to Viewport
                else { state = AppState::Exit; } // Exit app
                break;
            }

            /*
            case AppState::Viewport: {
                Viewport viewport;
                if (viewport.init()) { viewport.Terminate(); state = AppState::VEditor; } // Go back to Editor
                else { state = AppState::Exit; } // Exit app
                break;
            }*/

            default:
                state = AppState::Exit;
                break;
        }
    }
    return 0;
}