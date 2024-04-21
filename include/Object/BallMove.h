#pragma once
#include "Component/MoveComponent.h"

namespace Knuckles {
class BallMove : public MoveComponent {
public:
  BallMove(class Object *owner);

  void Update(float deltaTime) override;

  TypeID GetType() const override { return TBallMove; }

protected:
};
} // namespace Knuckles
