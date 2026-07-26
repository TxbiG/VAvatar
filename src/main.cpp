#include "editor/VHub.h"
#include "editor/VEditor.h"
#include "runtime/VStage.h"

enum class AppState { Hub, Editor, Stage, Exit };

/*
mkdir build
cd build
cmake ..
cmake --build .
*/
int main() {
    AppState state = AppState::Hub;

    // Main loop
    while (state != AppState::Exit) {
        switch (state) {
            case AppState::Hub: {
                VHub hub;
                switch (hub.init()) {
                    case HubResult::OpenEditor:
                        state = AppState::Editor;
                        break;
                    case HubResult::StartStage:
                        state = AppState::Stage;
                        break;
                    case HubResult::Exit:
                    default:
                        state = AppState::Exit;
                        break;
                }
                break;
            }

            case AppState::Editor: {
                VEditor editor;
                switch (editor.init()) {
                    case EditorResult::BackToHub:
                        state = AppState::Hub;
                        break;
                    case EditorResult::StartStage:
                        state = AppState::Stage;
                        break;
                    case EditorResult::Exit:
                    default:
                        state = AppState::Exit;
                        break;
                }
                break;
            }

            case AppState::Stage: {
                VStage stage;
                switch (stage.init()) {
                    case StageResult::BackToEditor:
                        state = AppState::Editor;
                        break;
                    case StageResult::Exit:
                    default:
                        state = AppState::Exit;
                        break;
                }
                break;
            }

            default:
                state = AppState::Exit;
                break;
        }
    }
    return 0;
}
