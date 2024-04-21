#include "UI/PauseMenu.h"
#include "Core/Application.h"
#include "UI/DialogBox.h"
#include <SDL.h>

namespace Knuckles {
PauseMenu::PauseMenu(Application *game) : UIScreen(game) {
  mApplication->SetState(Application::EPaused);
  SetRelativeMouseMode(false);
  SetTitle("PauseTitle");
  AddButton("ResumeButton", [this]() { Close(); });
  AddButton("QuitButton", [this]() {
    new DialogBox(mApplication, "QuitText",
                  [this]() { mApplication->SetState(Application::EQuit); });
  });
}

PauseMenu::~PauseMenu() {
  SetRelativeMouseMode(true);
  mApplication->SetState(Application::EApplicationplay);
}

void PauseMenu::HandleKeyPress(int key) {
  UIScreen::HandleKeyPress(key);

  if (key == SDLK_ESCAPE) {
    Close();
  }
}
} // namespace Knuckles
