#pragma once
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"

namespace Knuckles {

class ImGuiLayer {
public:
  ImGuiLayer(class Application *app);

  void Attach();
  void BeginFrame();

  void OnRender();

  void Shutdown();

private:
  class Application *mApp;
};
} // namespace Knuckles
