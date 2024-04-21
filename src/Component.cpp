#include "Component/Component.h"
#include "LevelLoader.h"
#include "Object/Object.h"

namespace Knuckles
{
const char *Component::TypeNames[NUM_COMPONENT_TYPES] = {
    "Component",       "AudioComponent",  "BallMove",
    "BoxComponent",    "CameraComponent", "FollowCamera",
    "MeshComponent",   "MoveComponent",   "SkeletalMeshComponent",
    "SpriteComponent", "MirrorCamera",    "PointLightComponent",
    "TargetComponent"};

Component::Component(Object *owner, int updateOrder)
    : mOwner(owner), mUpdateOrder(updateOrder) {
  // Add to Object's vector of components
  mOwner->AddComponent(this);
}

Component::~Component() { mOwner->RemoveComponent(this); }

void Component::Update(float deltaTime) {}

void Component::OnUpdateWorldTransform() {}

void Component::LoadProperties(const rapidjson::Value &inObj) {
  JsonHelper::GetInt(inObj, "updateOrder", mUpdateOrder);
}

void Component::SaveProperties(rapidjson::Document::AllocatorType &alloc,
                               rapidjson::Value &inObj) const {
  JsonHelper::AddInt(alloc, inObj, "updateOrder", mUpdateOrder);
}
}