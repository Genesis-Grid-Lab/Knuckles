#pragma once
#include "Object/Object.h"
#include <string>

namespace Knuckles {
class PlaneObject : public Object {
public:
  PlaneObject(class Application *game, std::string title);
  TypeID GetType() const override { return TPlaneObject; }

  Object *Clone(std::string title) override { return new PlaneObject(mApp, title);}
  class Mesh *GetMesh() override { return mMesh;}
  class MeshComponent *GetMeshComp() override { return mc;}
private:
  class Application *mApp;
  class Mesh *mMesh;
  class MeshComponent *mc;
};
} // namespace Knuckles
