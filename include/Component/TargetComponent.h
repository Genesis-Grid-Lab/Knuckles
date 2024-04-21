#pragma once
#include "Component/Component.h"

namespace Knuckles {
class TargetComponent : public Component {
public:
  TargetComponent(class Object *owner);
  ~TargetComponent();
  TypeID GetType() const override { return TTargetComponent; }
};
} // namespace Knuckles
