#include "Object/BallObject.h"
#include "Component//AudioComponent.h"
#include "Component/MeshComponent.h"
#include "Core/Application.h"
#include "LevelLoader.h"
#include "Object/BallMove.h"
#include "Renderer/Mesh.h"
#include "Renderer/Renderer.h"

namespace Knuckles
{
BallObject::BallObject(Application *game, std::string title) : Object(game, title), mLifeSpan(2.0f), mApp(game) {
  // SetScale(10.0f);
  MeshComponent *mc = new MeshComponent(this);
  Mesh *mesh = GetApplication()->GetRenderer()->GetMesh("Assets/Sphere.gpmesh");
  mc->SetMesh(mesh);
  BallMove *move = new BallMove(this);
  //move->SetForwardSpeed(1500.0f);
  mAudioComp = new AudioComponent(this);
}

void BallObject::UpdateObject(float deltaTime) {
  Object::UpdateObject(deltaTime);

}

void BallObject::HitTarget() { mAudioComp->PlayEvent("event:/Ding"); }

void BallObject::LoadProperties(const rapidjson::Value &inObj) {
  Object::LoadProperties(inObj);
  //JsonHelper::GetFloat(inObj, "lifespan", mLifeSpan);
}

void BallObject::SaveProperties(rapidjson::Document::AllocatorType &alloc,
                               rapidjson::Value &inObj) const {
  Object::SaveProperties(alloc, inObj);
  //JsonHelper::AddFloat(alloc, inObj, "lifespan", mLifeSpan);
}
}
