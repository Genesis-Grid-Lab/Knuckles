#pragma once
#include "Component/Component.h"
#include "Math.h"

namespace Knuckles {
class CameraComponent : public Component {
public:
  CameraComponent(class Object *owner, int updateOrder = 200);

  TypeID GetType() const override { return TCameraComponent; }

protected:
  void SetViewMatrix(const Matrix4 &view);
};
} // namespace Knuckles
