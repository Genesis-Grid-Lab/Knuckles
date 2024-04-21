#include "Component/CameraComponent.h"
#include "Audio/AudioSystem.h"
#include "Core/Application.h"
#include "Object/Object.h"
#include "Renderer/Renderer.h"

namespace Knuckles
{
CameraComponent::CameraComponent(Object *owner, int updateOrder)
    : Component(owner, updateOrder) {}

void CameraComponent::SetViewMatrix(const Matrix4 &view) {
  // Pass view matrix to renderer and audio system
  Application *game = mOwner->GetApplication();
  game->GetRenderer()->SetViewMatrix(view);
  game->GetAudioSystem()->SetListener(view);
}
}