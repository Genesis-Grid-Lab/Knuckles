#include "UI/DialogBox.h"
#include "Core/Application.h"
#include "Renderer/Renderer.h"

namespace Knuckles
{
DialogBox::DialogBox(Application *game, const std::string &text,
                     std::function<void()> onOK)
    : UIScreen(game) {
  // Adjust positions for dialog box
  mBGPos = Vector2(0.0f, 0.0f);
  mTitlePos = Vector2(0.0f, 100.0f);
  mNextButtonPos = Vector2(0.0f, 0.0f);

  mBackground = mApplication->GetRenderer()->GetTexture("Assets/DialogBG.png");
  SetTitle(text, Vector3::Zero, 30);
  AddButton("OKButton", [onOK]() { onOK(); });
  AddButton("CancelButton", [this]() { Close(); });
}

DialogBox::~DialogBox() {}
}