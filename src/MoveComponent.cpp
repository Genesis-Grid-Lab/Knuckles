#include "Component/MoveComponent.h"
#include "LevelLoader.h"
#include "Math.h"
#include "Object/Object.h"

namespace Knuckles {
MoveComponent::MoveComponent(class Object *owner, int updateOrder)
    : Component(owner, updateOrder), mAngularSpeed(0.0f), mForwardSpeed(0.0f) {}

void MoveComponent::Update(float deltaTime) {
  if (!Math::NearZero(mAngularSpeed)) {
    Quaternion rot = mOwner->GetRotation();
    float angle = mAngularSpeed * deltaTime;
    // Create quaternion for incremental rotation
    // (Rotate about up axis)
    Quaternion inc(Vector3::UnitZ, angle);
    // Concatenate old and new quaternion
    rot = Quaternion::Concatenate(rot, inc);
    mOwner->SetRotation(rot);
  }

  if (!Math::NearZero(mForwardSpeed) || !Math::NearZero(mStrafeSpeed) ||
      !Math::NearZero(mUpSpeed)) {
    Vector3 pos = mOwner->GetPosition();
    pos += mOwner->GetForward() * mForwardSpeed * deltaTime;
    pos += mOwner->GetRight() * mStrafeSpeed * deltaTime;
    pos += mOwner->GetUp() * mUpSpeed * deltaTime;
    mOwner->SetPosition(pos);
  }
}

void MoveComponent::LoadProperties(const rapidjson::Value &inObj) {
  Component::LoadProperties(inObj);

  JsonHelper::GetFloat(inObj, "angularSpeed", mAngularSpeed);
  JsonHelper::GetFloat(inObj, "forwardSpeed", mForwardSpeed);
  JsonHelper::GetFloat(inObj, "strafeSpeed", mStrafeSpeed);
  JsonHelper::GetFloat(inObj, "upSpeed", mUpSpeed);
}

void MoveComponent::SaveProperties(rapidjson::Document::AllocatorType &alloc,
                                   rapidjson::Value &inObj) const {
  Component::SaveProperties(alloc, inObj);

  JsonHelper::AddFloat(alloc, inObj, "angularSpeed", mAngularSpeed);
  JsonHelper::AddFloat(alloc, inObj, "forwardSpeed", mForwardSpeed);
  JsonHelper::AddFloat(alloc, inObj, "strafeSpeed", mStrafeSpeed);
  JsonHelper::AddFloat(alloc, inObj, "upSpeed", mUpSpeed);
}
} // namespace Knuckles