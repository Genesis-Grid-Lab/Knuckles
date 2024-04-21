#include "Object/CameraObject.h"
#include "Camera/FollowCamera.h"
#include "Camera/MirrorCamera.h"
#include "Component/MoveComponent.h"
#include "Component/SkeletalMeshComponent.h"
#include "Core/Application.h"
#include "Input/InputSystem.h"
#include "LevelLoader.h"
#include "Renderer/Renderer.h"
#include "SDL_scancode.h"

namespace Knuckles {
CameraObject::CameraObject(Application *game, std::string title)
    : Object(game, title), mMoving(false), mApp(game) {
  SetPosition(Vector3(0.0f, 0.0f, 0.0f));

  mMoveComp = new MoveComponent(this);
  mCameraComp = new FollowCamera(this);
  // mCameraComp->SnapToIdeal();

  // game->SetCameraObject(this);
}

void CameraObject::ObjectInput(const InputState &state) {
  float forwardSpeed = 0.0f;
  float angularSpeed = 0.0f;
  float upSpeed = 0.0f;
  // wasd movement
  if (state.Keyboard.GetKeyState(SDL_SCANCODE_W)) {
    forwardSpeed += 400.0f;
  }
  if (state.Keyboard.GetKeyState(SDL_SCANCODE_S)) {
    forwardSpeed -= 400.0f;
  }
  if (state.Keyboard.GetKeyState(SDL_SCANCODE_A)) {
    angularSpeed -= Math::Pi;
  }
  if (state.Keyboard.GetKeyState(SDL_SCANCODE_D)) {
    angularSpeed += Math::Pi;
  }
  if (state.Keyboard.GetKeyState(SDL_SCANCODE_SPACE)) {
    upSpeed += 400.0f;
  }
  if (state.Keyboard.GetKeyState(SDL_SCANCODE_LCTRL)) {
    upSpeed -= 400.0f;
  }

  mMoveComp->SetForwardSpeed(forwardSpeed);
  mMoveComp->SetAngularSpeed(angularSpeed);
  mMoveComp->SetUpSpeed(upSpeed);
}

void CameraObject::SetVisible(bool visible) { mMeshComp->SetVisible(visible); }

void CameraObject::LoadProperties(const rapidjson::Value &inObj) {
  Object::LoadProperties(inObj);
  JsonHelper::GetBool(inObj, "moving", mMoving);
}

void CameraObject::SaveProperties(rapidjson::Document::AllocatorType &alloc,
                                  rapidjson::Value &inObj) const {
  Object::SaveProperties(alloc, inObj);
  JsonHelper::AddBool(alloc, inObj, "moving", mMoving);
}
} // namespace Knuckles
