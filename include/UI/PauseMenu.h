#pragma once
#include "UI/UIScreen.h"

namespace Knuckles {
class PauseMenu : public UIScreen {
public:
  PauseMenu(class Application *game);
  ~PauseMenu();

  void HandleKeyPress(int key) override;
};
} // namespace Knuckles
