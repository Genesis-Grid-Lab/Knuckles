#include "Component/TargetComponent.h"
#include "Component/BoxComponent.h"
#include "Core/Application.h"
#include "Math.h"
#include "Object/Object.h"
#include "Renderer/PhysWorld.h"
#include "UI/HUD.h"

namespace Knuckles
{
TargetComponent::TargetComponent(Object *owner) : Component(owner) {
  mOwner->GetApplication()->GetHUD()->AddTargetComponent(this);
}

TargetComponent::~TargetComponent() {
  mOwner->GetApplication()->GetHUD()->RemoveTargetComponent(this);
}
}