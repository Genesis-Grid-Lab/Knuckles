#include "Core/Application.h"
#include "Animation/Animation.h"
#include "Animation/Skeleton.h"
#include "Audio/AudioSystem.h"
#include "Component/MeshComponent.h"
#include "Component/PointLightComponent.h"
#include "ImGui/ImguiL.h"
#include "Input/InputSystem.h"
#include "LevelLoader.h"
#include "Log/Log.h"
#include "Object/BallObject.h"
#include "Object/FollowObject.h"
#include "Object/Object.h"
#include "Object/PlaneObject.h"
#include "Object/TargetObject.h"
#include "Renderer/PhysWorld.h"
#include "Renderer/Renderer.h"
#include "SDL_stdinc.h"
#include "UI/Font.h"
#include "UI/HUD.h"
#include "UI/PauseMenu.h"
#include "UI/UIScreen.h"
#include "imgui.h"
#include <SDL_ttf.h>
#include <algorithm>
#include <rapidjson/document.h>

namespace Knuckles {
Application::Application(std::string title, int width, int height)
    : mRenderer(nullptr), mAudioSystem(nullptr), mPhysWorld(nullptr),
      mApplicationState(EApplicationplay), mUpdatingObjects(false),
      mTitle(title), mWidth(width), mHeight(height) {}

bool Application::Initialize() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
    KE_CORE_ERROR("Unable to initialize SDL: %s", SDL_GetError());
    return false;
  }

  // Create the renderer
  mRenderer = new Renderer(this);
  if (!mRenderer->Initialize(mTitle, mWidth, mHeight)) {
    KE_CORE_ERROR("Failed to initialize renderer");
    delete mRenderer;
    mRenderer = nullptr;
    return false;
  }

  // Create the input system
  mInputSystem = new InputSystem();
  if (!mInputSystem->Initialize()) {
    KE_CORE_ERROR("Failed to initialize input system");
    return false;
  }

  // Create the audio system
  mAudioSystem = new AudioSystem(this);
  if (!mAudioSystem->Initialize()) {
    KE_CORE_ERROR("Failed to initialize audio system");
    mAudioSystem->Shutdown();
    delete mAudioSystem;
    mAudioSystem = nullptr;
    return false;
  }

  // Create the physics world
  mPhysWorld = new PhysWorld(this);

  mImGuiLayer = new ImGuiLayer(this);

  mImGuiLayer->Attach();

  // Initialize SDL_ttf
  if (TTF_Init() != 0) {
    KE_CORE_ERROR("Failed to initialize SDL_ttf");
    return false;
  }

  LoadData();

  mTicksCount = SDL_GetTicks();

  return true;
}

void Application::RunLoop() {
  KE_CORE_INFO("LOOP START");
  while (mApplicationState != EQuit) {
    ProcessInput();
    UpdateApplication();
    GenerateOutput();
  }
  KE_CORE_INFO("LOOP END");
}

void Application::ProcessInput() {
  mInputSystem->PrepareForUpdate();
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    ImGui_ImplSDL2_ProcessEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
      mApplicationState = EQuit;
      break;
    // This fires when a key's initially pressed
    case SDL_KEYDOWN:
      if (!event.key.repeat) {
        if (mApplicationState == EApplicationplay) {
          HandleKeyPress(event.key.keysym.sym);
        } else if (!mUIStack.empty()) {
          if (mApplicationState == EApplicationplay ||
              mApplicationState == EPaused)
            mUIStack.back()->HandleKeyPress(event.key.keysym.sym);
        } else if (mApplicationState == EEditor) {
        }
      }
      break;
    case SDL_MOUSEBUTTONDOWN:
      if (mApplicationState == EApplicationplay) {
        HandleKeyPress(event.button.button);

      } else if (!mUIStack.empty()) {
        if (mApplicationState == EApplicationplay ||
            mApplicationState == EPaused)
          mUIStack.back()->HandleKeyPress(event.button.button);
      } else if (mApplicationState == EEditor) {
      }
      break;
    default:
      break;
    }
  }

  mInputSystem->Update();
  const InputState &state = mInputSystem->GetState();
  if (mApplicationState == EApplicationplay) {
    for (auto object : mObjects) {
      if (object->GetState() == Object::EActive) {
        object->ProcessInput(state);
      }
    }
    AppInput(state);
  } else if (!mUIStack.empty()) {
    if (mApplicationState == EApplicationplay || mApplicationState == EPaused)
      mUIStack.back()->ProcessInput(state);
  } else if (mApplicationState == EEditor) {
  }
}

void Application::HandleKeyPress(int key) {
  switch (key) {
  case SDLK_ESCAPE:
    // Create pause menu
    new PauseMenu(this);
    break;
  case '-': {
    // Reduce master volume
    float volume = mAudioSystem->GetBusVolume("bus:/");
    volume = Math::Max(0.0f, volume - 0.1f);
    mAudioSystem->SetBusVolume("bus:/", volume);
    break;
  }
  case '=': {
    // Increase master volume
    float volume = mAudioSystem->GetBusVolume("bus:/");
    volume = Math::Min(1.0f, volume + 0.1f);
    mAudioSystem->SetBusVolume("bus:/", volume);
    break;
  }
  case '1': {
    // Load English text
    LoadText("Assets/English.gptext");
    break;
  }
  case '2': {
    // Load Russian text
    LoadText("Assets/Russian.gptext");
    break;
  }
  case 'r': {
    // Save level
    LevelLoader::SaveLevel(this, "Assets/Saved.Klevel");
    break;
  }
  case SDL_BUTTON_LEFT: {
    break;
  }
  default:
    break;
  }
}

void Application::UpdateApplication() {
  // Compute delta time
  // Wait until 16ms has elapsed since last frame
  while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
    ;

  float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
  if (deltaTime > 0.05f) {
    deltaTime = 0.05f;
  }
  mTicksCount = SDL_GetTicks();

  if (mApplicationState == EApplicationplay) {
    // Update all objects
    mUpdatingObjects = true;
    for (auto object : mObjects) {
      object->Update(deltaTime);
    }
    mUpdatingObjects = false;

    // Move any pending objects to mObjects
    for (auto pending : mPendingObjects) {
      pending->ComputeWorldTransform();
      mObjects.emplace_back(pending);
    }
    mPendingObjects.clear();

    // Add any dead objects to a temp vector
    std::vector<Object *> deadObjects;
    for (auto object : mObjects) {
      if (object->GetState() == Object::EDead) {
        deadObjects.emplace_back(object);
      }
    }

    // Delete dead objects (which removes them from mObjects)
    for (auto object : deadObjects) {
      delete object;
    }
  }

  // Update audio system
  mAudioSystem->Update(deltaTime);

  // Update UI screens
  for (auto ui : mUIStack) {
    if (ui->GetState() == UIScreen::EActive) {
      ui->Update(deltaTime);
    }
  }
  // Delete any UIScreens that are closed
  auto iter = mUIStack.begin();
  while (iter != mUIStack.end()) {
    if ((*iter)->GetState() == UIScreen::EClosing) {
      delete *iter;
      iter = mUIStack.erase(iter);
    } else {
      ++iter;
    }
  }
}

void Application::GenerateOutput() {
  mImGuiLayer->BeginFrame();
  ImGuiLayerLoad();
  mRenderer->Draw();
}

void Application::LoadData() {
  KE_CORE_INFO("LOADING DATA");
  // Load English text
  LoadText("Assets/English.gptext");
  AppLoad();
  // Create HUD
  // mHUD = new HUD(this);

  // Enable relative mouse mode for camera look
  // SDL_SetRelativeMouseMode(SDL_TRUE);
  // Make an initial call to get relative to clear out
  // SDL_GetRelativeMouseState(nullptr, nullptr);
  KE_CORE_INFO("LOADING DATA DONE");
}

void Application::UnloadData() {
  KE_CORE_INFO("UNLOADING");
  // Delete objects
  // Because ~Object calls RemoveObject, have to use a different style loop
  while (!mObjects.empty()) {
    delete mObjects.back();
  }

  KE_CORE_INFO("CLEAR UI STACK");
  // Clear the UI stack
  while (!mUIStack.empty()) {
    delete mUIStack.back();
    mUIStack.pop_back();
  }

  if (mRenderer) {
    mRenderer->UnloadData();
  }

  KE_CORE_INFO("CLEAR FONT");
  // Unload fonts
  for (auto f : mFonts) {
    f.second->Unload();
    delete f.second;
  }

  KE_CORE_INFO("CLEAR SKELETONS");
  // Unload skeletons
  for (auto s : mSkeletons) {
    delete s.second;
  }

  KE_CORE_INFO("CLEAR ANIMATIONS");
  // Unload animations
  for (auto a : mAnims) {
    delete a.second;
  }
  KE_CORE_INFO("UNLOADING DONE");
}

void Application::Shutdown() {
  KE_CORE_INFO("SHUTING DOWN");
  UnloadData();
  KE_CORE_INFO("SHUTING IMGUILAYER");
  mImGuiLayer->Shutdown();
  KE_CORE_INFO("SHUTING IMGUILAYER DONE");
  TTF_Quit();
  delete mPhysWorld;
  if (mRenderer) {
    mRenderer->Shutdown();
  }
  if (mAudioSystem) {
    mAudioSystem->Shutdown();
  }
  SDL_Quit();
  KE_CORE_INFO("SHUTING DOWN DONE");
}

void Application::AddObject(Object *object) {
  // If we're updating objects, need to add to pending
  if (mUpdatingObjects) {
    mPendingObjects.emplace_back(object);
  } else {
    mObjects.emplace_back(object);
  }
}

void Application::RemoveObject(Object *object) {
  KE_CORE_INFO("REMOVING OBJECT");
  // Is it in pending objects?
  auto iter = std::find(mPendingObjects.begin(), mPendingObjects.end(), object);
  if (iter != mPendingObjects.end()) {
    // Swap to end of vector and pop off (avoid erase copies)
    std::iter_swap(iter, mPendingObjects.end() - 1);
    mPendingObjects.pop_back();
  }

  // Is it in objects?
  iter = std::find(mObjects.begin(), mObjects.end(), object);
  if (iter != mObjects.end()) {
    // Swap to end of vector and pop off (avoid erase copies)
    std::iter_swap(iter, mObjects.end() - 1);
    mObjects.pop_back();
  }
  KE_CORE_INFO("REMOVING OBJECT DONE");
}

void Application::PushUI(UIScreen *screen) { mUIStack.emplace_back(screen); }

Font *Application::GetFont(const std::string &fileName) {
  auto iter = mFonts.find(fileName);
  if (iter != mFonts.end()) {
    return iter->second;
  } else {
    Font *font = new Font(this);
    if (font->Load(fileName)) {
      mFonts.emplace(fileName, font);
    } else {
      font->Unload();
      delete font;
      font = nullptr;
    }
    return font;
  }
}

void Application::LoadText(const std::string &fileName) {
  // Clear the existing map, if already loaded
  mText.clear();

  rapidjson::Document doc;
  if (!LevelLoader::LoadJSON(fileName, doc)) {
    KE_CORE_ERROR("Failed to load text file {0}", fileName.c_str());
    return;
  }

  // Parse the text map
  const rapidjson::Value &actions = doc["TextMap"];
  for (rapidjson::Value::ConstMemberIterator itr = actions.MemberBegin();
       itr != actions.MemberEnd(); ++itr) {
    if (itr->name.IsString() && itr->value.IsString()) {
      mText.emplace(itr->name.GetString(), itr->value.GetString());
    }
  }
}

const std::string &Application::GetText(const std::string &key) {
  static std::string errorMsg("**KEY NOT FOUND**");
  // Find this text in the map, if it exists
  auto iter = mText.find(key);
  if (iter != mText.end()) {
    return iter->second;
  } else {
    return errorMsg;
  }
}

Skeleton *Application::GetSkeleton(const std::string &fileName) {
  auto iter = mSkeletons.find(fileName);
  if (iter != mSkeletons.end()) {
    return iter->second;
  } else {
    Skeleton *sk = new Skeleton();
    if (sk->Load(fileName)) {
      mSkeletons.emplace(fileName, sk);
    } else {
      delete sk;
      sk = nullptr;
    }
    return sk;
  }
}

Animation *Application::GetAnimation(const std::string &fileName) {
  auto iter = mAnims.find(fileName);
  if (iter != mAnims.end()) {
    return iter->second;
  } else {
    Animation *anim = new Animation();
    if (anim->Load(fileName)) {
      mAnims.emplace(fileName, anim);
    } else {
      delete anim;
      anim = nullptr;
    }
    return anim;
  }
}
} // namespace Knuckles
