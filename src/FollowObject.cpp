#include "Object/FollowObject.h"
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
FollowObject::FollowObject(Application *game, std::string title)
    : Object(game, title), mMoving(false), mApp(game) {
  mMeshComp = new SkeletalMeshComponent(this);
  mMeshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/CatWarrior.gpmesh"));
  mMeshComp->SetSkeleton(game->GetSkeleton("Assets/CatWarrior.gpskel"));
  mMeshComp->PlayAnimation(game->GetAnimation("Assets/CatActionIdle.gpanim"));
  SetPosition(Vector3(0.0f, 0.0f, 0.0f));

  mMoveComp = new MoveComponent(this);
  mCameraComp = new FollowCamera(this);
  mCameraComp->SnapToIdeal();

  // Add a component for the mirror camera
  MirrorCamera *mirror = new MirrorCamera(this);
  mirror->SnapToIdeal();

  game->SetFollowObject(this);
}

void FollowObject::ObjectInput(const InputState &state) {
  float forwardSpeed = 0.0f;
  float angularSpeed = 0.0f;
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

  // Did we just start moving?
  if (!mMoving && !Math::NearZero(forwardSpeed)) {
    mMoving = true;
    mMeshComp->PlayAnimation(
        GetApplication()->GetAnimation("Assets/CatRunSprint.gpanim"), 1.25f);
  }
  // Or did we just stop moving?
  else if (mMoving && Math::NearZero(forwardSpeed)) {
    mMoving = false;
    mMeshComp->PlayAnimation(
        GetApplication()->GetAnimation("Assets/CatActionIdle.gpanim"));
  }
  mMoveComp->SetForwardSpeed(forwardSpeed);
  mMoveComp->SetAngularSpeed(angularSpeed);
}

void FollowObject::SetVisible(bool visible) { mMeshComp->SetVisible(visible); }

void FollowObject::LoadProperties(const rapidjson::Value &inObj) {
  Object::LoadProperties(inObj);
  JsonHelper::GetBool(inObj, "moving", mMoving);
}

void FollowObject::SaveProperties(rapidjson::Document::AllocatorType &alloc,
                                  rapidjson::Value &inObj) const {
  Object::SaveProperties(alloc, inObj);
  JsonHelper::AddBool(alloc, inObj, "moving", mMoving);
}
} // namespace Knuckles
