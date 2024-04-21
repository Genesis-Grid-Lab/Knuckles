#include "Object/PlaneObject.h"
#include "Component/BoxComponent.h"
#include "Component/MeshComponent.h"
#include "Core/Application.h"
#include "Renderer/Mesh.h"
#include "Renderer/Renderer.h"

namespace Knuckles
{
PlaneObject::PlaneObject(Application *game, std::string title) : Object(game, title), mApp(game) {
  SetScale(10.0f);
  mc = new MeshComponent(this);
  mMesh = GetApplication()->GetRenderer()->GetMesh("Assets/Plane.gpmesh");
  mc->SetMesh(mMesh);
  // Add collision box
  BoxComponent *bc = new BoxComponent(this);
  bc->SetObjectBox(mMesh->GetBox());
}
}
