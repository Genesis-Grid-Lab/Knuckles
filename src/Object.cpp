#include "Object/Object.h"
#include "Component/Component.h"
#include "Core/Application.h"
#include "Input/InputSystem.h"
#include "LevelLoader.h"
#include <algorithm>

namespace Knuckles {
const char *Object::TypeNames[NUM_OBJECT_TYPES] = {
    "Object", "BallObject", "FollowObject", "PlaneObject", "TargetObject",
};

Object::Object(Application *game, std::string title)
    : mState(EActive), mPosition(Vector3::Zero),
      mRotation(Quaternion::Identity), mScale(1.0f), mApplication(game),
      mRecomputeTransform(true), mTitle(title) {
  mApplication->AddObject(this);
}

Object::~Object() {
  mApplication->RemoveObject(this);
  // Need to delete components
  // Because ~Component calls RemoveComponent, need a different style loop
  while (!mComponents.empty()) {
    delete mComponents.back();
  }
}

void Object::Update(float deltaTime) {
  if (mState == EActive) {
    if (mRecomputeTransform) {
      ComputeWorldTransform();
    }
    UpdateComponents(deltaTime);
    UpdateObject(deltaTime);
  }
}

void Object::UpdateComponents(float deltaTime) {
  for (auto comp : mComponents) {
    comp->Update(deltaTime);
  }
}

void Object::UpdateObject(float deltaTime) {}

void Object::ProcessInput(const InputState &state) {
  if (mState == EActive) {
    // First process input for components
    for (auto comp : mComponents) {
      comp->ProcessInput(state);
    }

    ObjectInput(state);
  }
}

void Object::ObjectInput(const InputState &state) {}

void Object::ComputeWorldTransform() {
  mRecomputeTransform = false;
  // Scale, then rotate, then translate
  mWorldTransform = Matrix4::CreateScale(mScale);
  mWorldTransform *= Matrix4::CreateFromQuaternion(mRotation);
  mWorldTransform *= Matrix4::CreateTranslation(mPosition);

  // Inform components world transform updated
  for (auto comp : mComponents) {
    comp->OnUpdateWorldTransform();
  }
}

void Object::RotateToNewForward(const Vector3 &forward) {
  // Figure out difference between original (unit x) and new
  float dot = Vector3::Dot(Vector3::UnitX, forward);
  float angle = Math::Acos(dot);
  // Facing down X
  if (dot > 0.9999f) {
    SetRotation(Quaternion::Identity);
  }
  // Facing down -X
  else if (dot < -0.9999f) {
    SetRotation(Quaternion(Vector3::UnitZ, Math::Pi));
  } else {
    // Rotate about axis from cross product
    Vector3 axis = Vector3::Cross(Vector3::UnitX, forward);
    axis.Normalize();
    SetRotation(Quaternion(axis, angle));
  }
}

void Object::AddComponent(Component *component) {
  // Find the insertion point in the sorted vector
  // (The first element with a order higher than me)
  int myOrder = component->GetUpdateOrder();
  auto iter = mComponents.begin();
  for (; iter != mComponents.end(); ++iter) {
    if (myOrder < (*iter)->GetUpdateOrder()) {
      break;
    }
  }

  // Inserts element before position of iterator
  mComponents.insert(iter, component);
}

void Object::RemoveComponent(Component *component) {
  auto iter = std::find(mComponents.begin(), mComponents.end(), component);
  if (iter != mComponents.end()) {
    mComponents.erase(iter);
  }
}

void Object::LoadProperties(const rapidjson::Value &inObj) {
  // Use strings for different states
  std::string state;
  if (JsonHelper::GetString(inObj, "state", state)) {
    if (state == "active") {
      SetState(EActive);
    } else if (state == "paused") {
      SetState(EPaused);
    } else if (state == "dead") {
      SetState(EDead);
    }
  }
  JsonHelper::GetString(inObj, "title", mTitle);
  JsonHelper::GetInt(inObj, "ID", mID);
  // Load position, rotation, and scale, and compute transform
  JsonHelper::GetVector3(inObj, "position", mPosition);
  JsonHelper::GetQuaternion(inObj, "rotation", mRotation);
  JsonHelper::GetFloat(inObj, "scale", mScale);
  ComputeWorldTransform();
}

void Object::SaveProperties(rapidjson::Document::AllocatorType &alloc,
                            rapidjson::Value &inObj) const {
  std::string state = "active";
  if (mState == EPaused) {
    state = "paused";
  } else if (mState == EDead) {
    state = "dead";
  }

  JsonHelper::AddString(alloc, inObj, "title", mTitle);
  JsonHelper::AddInt(alloc, inObj, "ID", mID);
  JsonHelper::AddString(alloc, inObj, "state", state);
  JsonHelper::AddVector3(alloc, inObj, "position", mPosition);
  JsonHelper::AddQuaternion(alloc, inObj, "rotation", mRotation);
  JsonHelper::AddFloat(alloc, inObj, "scale", mScale);
}
} // namespace Knuckles
