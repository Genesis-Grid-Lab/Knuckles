#pragma once
#define SDL_MAIN_HANDLED
#include <cstdio>
extern Knuckles::Application *Knuckles::CreateApplication();

int main(int argc, char* argv[]) {
  printf("Init Knuckles Engine!\n");
  Knuckles::Log::Init();
  KE_CORE_WARN("Log init");
  KE_INFO("Client log init");
  auto app = Knuckles::CreateApplication();
  bool success = app->Initialize();
  if (success) {
    app->RunLoop();
  }
  app->Shutdown();
  return 0;
}
