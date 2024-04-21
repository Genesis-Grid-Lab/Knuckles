#pragma once
#include "Audio/SoundEvent.h"
#include "Math.h"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace Knuckles {
class Application {
public:
  Application(std::string title, int width, int height);
  bool Initialize();
  void RunLoop();
  void Shutdown();

  void AddObject(class Object *Object);
  void RemoveObject(class Object *Object);

  void AddMusic(SoundEvent *music);
  void RemoveMusics(SoundEvent *music);

  virtual void AppLoad() {}
  virtual void ImGuiLayerLoad() {}
  virtual void AppInput(const struct InputState &state) {}

  class Renderer *GetRenderer() { return mRenderer; }
  class AudioSystem *GetAudioSystem() { return mAudioSystem; }
  class PhysWorld *GetPhysWorld() { return mPhysWorld; }
  class InputSystem *GetInputSystem() { return mInputSystem; }
  class HUD *GetHUD() { return mHUD; }

  // Manage UI stack
  const std::vector<class UIScreen *> &GetUIStack() { return mUIStack; }
  void PushUI(class UIScreen *screen);

  class FollowObject *GetPlayer() { return mFollowObject; }

  enum ApplicationState {
    EApplicationplay,
    EPaused,
    EQuit,
    EEditor,
  };

  ApplicationState GetState() const { return mApplicationState; }
  void SetState(ApplicationState state) { mApplicationState = state; }

  class Font *GetFont(const std::string &fileName);

  void LoadText(const std::string &fileName);
  const std::string &GetText(const std::string &key);

  class Skeleton *GetSkeleton(const std::string &fileName);

  class Animation *GetAnimation(const std::string &fileName);

  const std::vector<class Object *> &GetObjects() const { return mObjects; }
  const std::vector<SoundEvent *> &GetMusics() const { return mMusics; }
  void SetFollowObject(class FollowObject *Object) { mFollowObject = Object; }

private:
  void ProcessInput();
  void HandleKeyPress(int key);
  void UpdateApplication();
  void GenerateOutput();
  void LoadData();
  void UnloadData();

  // All the Objects in the game
  std::vector<class Object *> mObjects;
  std::vector<class UIScreen *> mUIStack;
  std::vector<SoundEvent *> mMusics;
  // Map for fonts
  std::unordered_map<std::string, class Font *> mFonts;
  // Map of loaded skeletons
  std::unordered_map<std::string, class Skeleton *> mSkeletons;
  // Map of loaded animations
  std::unordered_map<std::string, class Animation *> mAnims;

  // Map for text localization
  std::unordered_map<std::string, std::string> mText;
  // Any pending Objects
  std::vector<class Object *> mPendingObjects;

  class Renderer *mRenderer;
  class AudioSystem *mAudioSystem;
  class InputSystem *mInputSystem;
  class PhysWorld *mPhysWorld;
  class HUD *mHUD;
  class ImGuiLayer *mImGuiLayer;

  uint32_t mTicksCount;
  ApplicationState mApplicationState;
  // Track if we're updating Objects right now
  bool mUpdatingObjects;

  std::string mTitle;
  int mWidth, mHeight;

  // Application-specific code
  class FollowObject *mFollowObject;
  class SpriteComponent *mCrosshair;
};
// To be define in client
Application *CreateApplication();
} // namespace Knuckles
