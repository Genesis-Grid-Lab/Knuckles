#pragma once
#include "UI/UIScreen.h"

namespace Knuckles {
class DialogBox : public UIScreen {
public:
  // (Lower draw order corresponds with further back)
  DialogBox(class Application *game, const std::string &text,
            std::function<void()> onOK);
  ~DialogBox();
};
} // namespace Knuckles
