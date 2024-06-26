#include "Component/BoxComponent.h"
#include "Core/Application.h"
#include "LevelLoader.h"
#include "Object/Object.h"
#include "Renderer/PhysWorld.h"

namespace Knuckles
{
BoxComponent::BoxComponent(Object *owner, int updateOrder)
    : Component(owner, updateOrder), mObjectBox(Vector3::Zero, Vector3::Zero),
      mWorldBox(Vector3::Zero, Vector3::Zero), mShouldRotate(true) {
  mOwner->GetApplication()->GetPhysWorld()->AddBox(this);
}

BoxComponent::~BoxComponent() {
  mOwner->GetApplication()->GetPhysWorld()->RemoveBox(this);
}

void BoxComponent::OnUpdateWorldTransform() {
  // Reset to object space box
  mWorldBox = mObjectBox;
  // Scale
  mWorldBox.mMin *= mOwner->GetScale();
  mWorldBox.mMax *= mOwner->GetScale();
  // Rotate (if we want to)
  if (mShouldRotate) {
    mWorldBox.Rotate(mOwner->GetRotation());
  }
  // Translate
  mWorldBox.mMin += mOwner->GetPosition();
  mWorldBox.mMax += mOwner->GetPosition();
}

void BoxComponent::LoadProperties(const rapidjson::Value &inObj) {
  Component::LoadProperties(inObj);

  JsonHelper::GetVector3(inObj, "objectMin", mObjectBox.mMin);
  JsonHelper::GetVector3(inObj, "objectMax", mObjectBox.mMax);
  JsonHelper::GetVector3(inObj, "worldMin", mWorldBox.mMin);
  JsonHelper::GetVector3(inObj, "worldMax", mWorldBox.mMax);
  JsonHelper::GetBool(inObj, "shouldRotate", mShouldRotate);
}

void BoxComponent::SaveProperties(rapidjson::Document::AllocatorType &alloc,
                                  rapidjson::Value &inObj) const {
  Component::SaveProperties(alloc, inObj);

  JsonHelper::AddVector3(alloc, inObj, "objectMin", mObjectBox.mMin);
  JsonHelper::AddVector3(alloc, inObj, "objectMax", mObjectBox.mMax);
  JsonHelper::AddVector3(alloc, inObj, "worldMin", mWorldBox.mMin);
  JsonHelper::AddVector3(alloc, inObj, "worldMax", mWorldBox.mMax);
  JsonHelper::AddBool(alloc, inObj, "shouldRotate", mShouldRotate);
}
}