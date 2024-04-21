#include "Object/TargetObject.h"
#include "Component/BoxComponent.h"
#include "Component/MeshComponent.h"
#include "Component/TargetComponent.h"
#include "Core/Application.h"
#include "Renderer/Mesh.h"
#include "Renderer/Renderer.h"
#include <string>

namespace Knuckles
{
TargetObject::TargetObject(Application *game, std::string title) : Object(game, title),mApp(game) {
  // SetScale(10.0f);
  SetRotation(Quaternion(Vector3::UnitZ, Math::Pi));
  MeshComponent *mc = new MeshComponent(this);
  Mesh *mesh = GetApplication()->GetRenderer()->GetMesh("Assets/Target.gpmesh");
  mc->SetMesh(mesh);
  // Add collision box
  BoxComponent *bc = new BoxComponent(this);
  bc->SetObjectBox(mesh->GetBox());
  new TargetComponent(this);
}
}
