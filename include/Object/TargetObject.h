#pragma once
#include "Object/Object.h"
#include <string>

namespace Knuckles {
class TargetObject : public Object {
public:
  TargetObject(class Application *game, std::string title);
  TypeID GetType() const override { return TTargetObject; }

  Object *Clone(std::string title) override { return new TargetObject(mApp, title);}
private:
  class Application *mApp;
};
} // namespace Knuckles
