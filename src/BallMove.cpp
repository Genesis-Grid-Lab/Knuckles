#include "Object/BallMove.h"
#include "Core/Application.h"
#include "Object/Object.h"
#include "Object/BallObject.h"
#include "Object/TargetObject.h"
#include "Renderer/PhysWorld.h"

namespace Knuckles
{
BallMove::BallMove(Object *owner) : MoveComponent(owner) {}

void BallMove::Update(float deltaTime) {
  const float segmentLength = 30.0f;
  PhysWorld *phys = mOwner->GetApplication()->GetPhysWorld();

  // Construct segment in direction of travel
  Vector3 start = mOwner->GetPosition();
  Vector3 dir = mOwner->GetForward();
  Vector3 end = start + dir * segmentLength;
  // Create line segment
  LineSegment l(start, end);
  // Test segment vs world
  PhysWorld::CollisionInfo info;
  if (phys->SegmentCast(l, info)) {
    // If we collided, reflect the ball about the normal
    dir = Vector3::Reflect(dir, info.mNormal);
    mOwner->RotateToNewForward(dir);
    // Did we hit a target?
    TargetObject *target = dynamic_cast<TargetObject *>(info.mObject);
    if (target) {
      static_cast<BallObject *>(mOwner)->HitTarget();
    }
  }
  MoveComponent::Update(deltaTime);
}
}